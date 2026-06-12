#!/usr/bin/env python3
"""
acquire.py - Start a Z+F PROFILER, stream live points for 30 s, then stop.

Prints real-time   t  x  y  z  intensity   for each (subsampled) measured point.

Built from the Z+F SDK (lrcsocket + zfxyz). Two TCP sockets to the scanner:

  * COMMAND  - TCP port 6100 (zfports.h PORT_COMMAND). Commands are UTF-8
               terminated with CRLF '\\r\\n' (confirmed on-device); replies are XML.
  * STREAM   - TCP port 6105 (PORT_XYZTCP / PORT_TCPZIP). Each message is framed
               by a 14-byte header (zfxyz::header):
                   marker "Z+F\\0" (4) | type u16 | size u32 | elems u32   (LE)
               payload types (zfxyz_wire.hpp):
                   ZFS_HEADER_ID=4  - the .zfs TLV header (sent once at start)
                   LINE_HEADER_ID=3 - per-line header (52 B: id,enc,time,linenr...)
                   RAW_PIXEL_ID=6   - pixel data, uncompressed (-linestream tcp)
                   COMP_PIXEL_ID=7  - pixel data, zlib (-linestream tcpzip)

Scan command - mirrors THIS device's own recorded working command (from the .zfs
metadata: numeric resolution/quality, -path scans, -memorize 3, -statusstream),
plus -linestream <mode> for TCP streaming and -writing 0 (no disk write):
    scan -profiler -resolution 10000 -quality 50 -file live_<timestamp> -path scans
         -filesize 250mb -linestream tcpzip -statusstream -memorize 3
         -writing 0 -ip <local-ip>
A unique '-file live_<timestamp>' is used each run so the file never pre-exists
and the scanner never pops a yes/no "Overwrite?" box that would block headless
acquisition. (Override with --file or --scan-args; diagnose with --probe/--cmd.)
As a safety net, any yes/no messagebox that does appear is auto-answered.
Motor speed / data rate: --rps and --mhz append the (firmware-side) speed/datarate
flags; on this device they are otherwise governed by --resolution / --quality.

Decoding reuses the validated .zfs logic (see zfs2ply.py): each streamed line is
one full vertical-circle revolution; for pixel i of a line:
    vertical angle a = 2*pi * i / pixel
    range  r = ch0 * 0.0001 m            (1/10 mm raw unit, per SDK)
    cross-section  Y = -r*cos a , Z = -r*sin a  ;  X = line * spacing (synthetic; 0=stationary)
    intensity = ch1 ;  t = line-header time [ms] / 1000

-------------------------------------------------------------------------------
!!! WRITTEN FROM THE SDK SPEC - NOT TESTED AGAINST A REAL SCANNER. !!!
The ports, command syntax, scan command and range scale are SDK-validated; the
exact stream-frame endianness and message ordering are inferred from the headers
and may need small adjustments on real hardware. For guaranteed-correct
calibrated XYZ, link libzfs.so and call ZFLineByLine::XYZ() instead.
-------------------------------------------------------------------------------
"""
import sys
import re
import math
import socket
import time
import zlib
import struct
import argparse

try:
    import numpy as np
except ImportError:
    sys.exit("This script requires numpy:  pip install numpy")

PORT_COMMAND = 6100
PORT_STREAM  = 6105
MARKER       = b"Z+F\x00"

# zfxyz payload types
ZFS_HEADER_ID, LINE_HEADER_ID, RAW_PIXEL_ID, COMP_PIXEL_ID = 4, 3, 6, 7

# LineHeader uint32 indices (lineheader.h) -> byte offset = idx*4
LH_TIME, LH_LINENUMBER = 7, 8

RANGE_SCALE_M = 0.0001       # 1/10 mm raw range unit (Z+F SDK)

# Scan-command flags for motor speed / laser data-rate (device-firmware side;
# names inferred - override via --extra / --scan-args if rejected).
RPS_FLAG = "-speed"          # laser motor speed   [rev/s]
MHZ_FLAG = "-datarate"       # laser data rate     [MHz]
ROTATE_DEG = 90.8            # angle-zero/mount correction [deg]; calibrated 2026-06-10 (orientation user-verified)

SENTINEL_MIN_R = 0.345       # device min-range sentinel (~0.334 m) marks no-return
AMBIG_M = 6.5536             # fine-range ambiguity interval [m]
MIXPIX_DEG = 8.0             # mixed-pixel filter: drop points whose local chord
                             # is within this angle of the beam (edge trails)


def mixpix_mask(r, valid, rot_rad, deg=MIXPIX_DEG, W=4):
    """Mixed-pixel (edge-trail) filter, SDK-style: phase scanners bridge depth
    edges with radial point trails. Drop valid points whose local surface chord
    lies within `deg` degrees of the beam direction - EXCEPT long runs of such
    points (a real surface at grazing incidence, e.g. the far floor, is also
    chord-radial but spans far more pixels than a spot-size-bounded edge
    trail). Then one 'enlarge' pass drops survivors with >=2 dropped
    neighbours within +/-2. Returns keep mask. Matches cpp/ZnF9020.cpp."""
    if deg <= 0:
        return valid
    MIX_RUN_EXEMPT, MIX_RUN_GAP = 32, 8
    n = r.size
    aa = 2.0 * np.pi * np.arange(n) / n + rot_rad
    py = -r * np.cos(aa); pz = -r * np.sin(aa)
    keep = valid.copy()
    cy = py[2*W:] - py[:-2*W]; cz = pz[2*W:] - pz[:-2*W]
    cn = np.hypot(cy, cz)
    dot = np.abs(cy * (-np.cos(aa[W:-W])) + cz * (-np.sin(aa[W:-W])))
    with np.errstate(invalid="ignore", divide="ignore"):
        g = np.degrees(np.arccos(np.clip(dot / np.where(cn > 1e-9, cn, np.inf), 0, 1)))
    ok = valid[W:-W] & valid[2*W:] & valid[:-2*W]
    flag = np.zeros(n, bool)
    flag[W:-W] = ok & (g < deg)
    drop = np.zeros(n, bool)
    fi = np.nonzero(flag)[0]
    if fi.size:                                # short runs only = edge trails
        cuts = np.nonzero(np.diff(fi) > MIX_RUN_GAP)[0] + 1
        for grp in np.split(fi, cuts):
            if grp[-1] - grp[0] < MIX_RUN_EXEMPT:
                drop[grp] = True
    nb = np.convolve(drop.astype(np.int8), np.array([1,1,0,1,1],np.int8), mode="same")
    drop |= valid & (nb >= 2)
    keep[drop] = False
    return keep



_PREV_R = {"v": None}        # last profile's full decode (temporal confirmation)


def decode_reset():
    """Reset per-scan decoder diagnostics (burst cooldown)."""
    _PREV_R["cool"] = 0


def decode_dual(u, pixel, unwrap=True, prev_sec=None, min_amp=2000.0, min_range=0.0):
    """ABSOLUTE per-pixel decode (matches cpp/ZnF9020.cpp decodeProfile).
    Live tcpzip layout, all values byte-doubled coarse (high byte duplicated):
      plane0[half+j]  range modulo 6.5536 m (x 0.1 mm, ~25.7 mm steps)
      plane1[j]       ABSOLUTE WRAP COUNT k (value/257; 0..27 = 182.68 m spec)
      plane1[half+j]  amplitude
      plane2[j]       secondary estimate (x 0.2 mm, modulo 13.107 m; "parks"
                      on exactly-repeating codes when it fails to measure)
    True range r = plane0 + k*6.5536 - absolute, no ambiguity, no cross-line
    state. plane2 is used only as a mixed-echo sanity check MODULO its own
    interval. No-return STATUS pixels: duplicated-byte code in plane0 AND
    plane2 == the same raw value. unwrap=False ignores k (fold display).
    prev_sec accepted for API compat (unused).

    NEAR-FIELD OVERLOAD (hand <30 cm; characterized from a raw capture
    2026-06-12): upset directions emit byte-soup, not measurements -
      * during the event, plane1 carries byte-doubled garbage codes (0x0505,
        0x1515, ...) that decode as valid k -> constant-radius ARCS at 38 /
        94 / 144 / 177 m, interleaved with mixed-byte (invalid-k) pixels;
      * after the event the directions stay PARKED for the rest of the scan
        at ~181.3-182 m with FROZEN amplitude (one byte-doubled code value,
        e.g. 0xB4B4, repeated +/- low-byte soup) - the "saturation scar".
    Two per-pixel gates remove both (measured: 99.6% of artifacts, 0.0015%
    genuine loss) so R_MAX is the full 182.68 m spec, not a band cut:
      AMP-PARK   amplitude exactly repeats >=3 times among +-4 neighbours
                 (>=2 if the value is byte-doubled): genuine amplitude
                 jitters; a frozen value marks a parked/scarred direction.
      UPSET-NEIGHBOURHOOD   a wrap claim (k>=1) within +-8 px of corruption
                 evidence (invalid-k pixel or amp-park) is garbage from the
                 same upset sector. k=0 pixels are unaffected; genuine far
                 returns sit in clean neighbourhoods and keep their k.
    Returns (r_metres, amp, sec); invalid -> r=0."""
    SEC_MOD_M, SEC_MIN_M, SEC_VETO_M = 13.1072, 0.40, 2.5
    PARKED_MIN = 3
    R_MAX_M = 182.68             # full spec range (ZFS header tag 50)
    AMP_PARK_MIN = 3             # exact amp repeats in +-4 => parked direction
    AMP_PARK_DUP_MIN = 2         # byte-doubled amp value needs only 2 (soup edges)
    UPSET_NB = 8                 # corruption-evidence influence radius [px]
    half = pixel // 2
    fraw = u[half:pixel].astype(np.int64)
    kraw = u[pixel:pixel + half].astype(np.int64)
    ampi = u[pixel + half:2 * pixel].astype(np.int64)
    amp  = ampi.astype(np.float64)
    sraw = u[2 * pixel:2 * pixel + half].astype(np.int64)
    sec  = sraw.astype(np.float64) * 2 * RANGE_SCALE_M
    # DEVICE RECALIBRATION BURST (~41 s in, ~60 lines): plane0 collapses to
    # one constant no-return code with ambient amplitude; drop lines whole
    # (no real scene puts >15% of a revolution into one coarse bin).
    modal = int(np.bincount(u[half:pixel], minlength=65536).max())
    if 100 * modal > 15 * half:
        _PREV_R["cool"] = 3
        return np.zeros(half), amp, sec
    # WRAP-COUNTER INTEGRITY: plane1[j] is byte-doubled in every healthy line
    # ever captured. A broken k channel (device upset after near-field
    # saturation) would scatter points to wild ranges - drop such lines whole.
    kr = np.floor(kraw / 257.0 + 0.5)
    kbad = np.abs(kraw - kr * 257) > 1
    if 20 * int(kbad.sum()) > half:
        _PREV_R["cool"] = 3
        return np.zeros(half), amp, sec
    if _PREV_R.get("cool", 0) > 0:
        _PREV_R["cool"] -= 1
        return np.zeros(half), amp, sec
    code = ((fraw & 0xFF) == (fraw >> 8)) & (sraw == fraw)   # no-return status
    fine = fraw.astype(np.float64) * RANGE_SCALE_M
    k = kr                                           # byte-doubled wrap count
    if not unwrap:
        k = np.zeros_like(k)
    r = fine + k * AMBIG_M
    # AMP-PARK: frozen amplitude = parked/scarred direction (genuine jitters)
    ampprk = np.zeros(half, dtype=np.int64)
    for off in (-4, -3, -2, -1, 1, 2, 3, 4):
        ampprk += (ampi == np.roll(ampi, off))
    ampdup = (ampi & 0xFF) == (ampi >> 8)
    amppark = (ampprk >= AMP_PARK_MIN) | (ampdup & (ampprk >= AMP_PARK_DUP_MIN))
    # UPSET-NEIGHBOURHOOD: dilate corruption evidence (invalid k / amp-park)
    ev = kbad | amppark
    near_upset = ev.copy()
    for off in range(1, UPSET_NB + 1):
        near_upset |= np.roll(ev, off) | np.roll(ev, -off)
    ok = ((~code) & (~kbad) & (amp > min_amp)
          & ~amppark                                 # saturation scar / park
          & ~((k >= 1) & near_upset)                 # wrap claim in upset sector
          & ~((k == 0) & (fine < SENTINEL_MIN_R))    # window sentinel
          & (r <= R_MAX_M) & (r >= min_range))
    if unwrap:
        # plane2 sanity: when it MEASURES (parked codes exactly repeat across
        # neighbours; real values jitter), it must agree with r modulo its
        # own interval, else the pixel is a mixed echo
        parked = np.zeros(half, dtype=np.int64)
        for off in (-4, -3, -2, -1, 1, 2, 3, 4):
            parked += (sraw == np.roll(sraw, off))
        secm = (sec >= SEC_MIN_M) & (parked < PARKED_MIN)
        m = np.mod(r, SEC_MOD_M)
        d = np.abs(sec - m)
        d = np.minimum(d, SEC_MOD_M - d)
        ok &= ~(secm & (d > SEC_VETO_M))
    return np.where(ok, r, 0.0), amp, sec


def parse_zfs_header_pixel(buf):
    """Walk the TLV header just enough to find pixel-per-line (idx 2)."""
    pos = 8
    try:
        while pos + 4 <= len(buf):
            tag = struct.unpack_from("<H", buf, pos)[0]
            length, typ = buf[pos + 2], buf[pos + 3]
            if tag == 0xFFFF:
                break
            if typ == 0x01:                                   # string
                pos += 16 + ((length + 15) // 16) * 16
                continue
            if typ == 0x00 and length in (1, 2, 4, 8, 9, 12, 16):
                if tag == 0x0002 and length == 4:
                    return struct.unpack_from("<I", buf, pos + 4)[0]
                pos += ((4 + length + 15) // 16) * 16
                continue
            break
    except struct.error:
        pass
    return None


def lineheader_time_ms(lh):
    if len(lh) >= (LH_TIME + 1) * 4:
        return struct.unpack_from("<I", lh, LH_TIME * 4)[0]
    return 0


def lineheader_number(lh):
    if len(lh) >= (LH_LINENUMBER + 1) * 4:
        return struct.unpack_from("<I", lh, LH_LINENUMBER * 4)[0]
    return -1


def decode_line(pixdata, pixel, line_idx, t_s, spacing, min_amp, every, rot_deg=None, min_range=0.0, unwrap=True, mixpix_deg=MIXPIX_DEG, prev_sec=None):
    """Decompressed pixel block -> list of (t,x,y,z,intensity) for kept points.
    Planar channels: ch0=range, ch1=intensity (validated against .zfs)."""
    u = np.frombuffer(pixdata, dtype="<u2")
    if pixel <= 0 or u.size < 4 * pixel:
        return []
    r_m, amp, _sec = decode_dual(u, pixel, unwrap=unwrap, prev_sec=prev_sec,
                                 min_amp=min_amp, min_range=min_range)
    pixel = pixel // 2                        # directions per revolution
    i = np.arange(pixel)
    keep = (amp > min_amp) & (r_m > 0) & (r_m >= min_range)
    keep = mixpix_mask(r_m, keep, np.radians(ROTATE_DEG if rot_deg is None else rot_deg),
                       deg=mixpix_deg)
    if every > 1:
        sel = np.zeros(pixel, bool)
        sel[::every] = True
        keep &= sel
    idx = np.nonzero(keep)[0]
    if idx.size == 0:
        return []
    a = 2.0 * np.pi * idx / pixel + np.radians(ROTATE_DEG if rot_deg is None else rot_deg)
    r = r_m[idx]
    y = -r * np.cos(a)          # Z+F manual frame (x fwd, y left, z up): +Y = -cos
    z = -r * np.sin(a)          # +Z = -sin
    x = float(line_idx) * spacing            # synthetic travel; 0 (default) = stationary
    return [(t_s, x, float(yy), float(zz), float(aa))
            for yy, zz, aa in zip(y, z, amp[idx])]


def decode_profile_yz(pixdata, pixel, min_amp, decimate=1, rot_deg=None, min_range=0.0, unwrap=True, mixpix_deg=MIXPIX_DEG, prev_sec=None):
    """One profile's pixel data -> (y, z, intensity) numpy arrays (cross-section,
    metres). Same model as decode_line but returns arrays for plotting."""
    u = np.frombuffer(pixdata, dtype="<u2")
    empty = (np.empty(0), np.empty(0), np.empty(0))
    if pixel <= 0 or u.size < 4 * pixel:
        return empty
    r_m, amp, _sec = decode_dual(u, pixel, unwrap=unwrap, prev_sec=prev_sec,
                                 min_amp=min_amp, min_range=min_range)
    pixel = pixel // 2                        # directions per revolution
    keep = (amp > min_amp) & (r_m > 0) & (r_m >= min_range)
    keep = mixpix_mask(r_m, keep, np.radians(ROTATE_DEG if rot_deg is None else rot_deg),
                       deg=mixpix_deg)
    if decimate > 1:
        sel = np.zeros(pixel, bool)
        sel[::decimate] = True
        keep &= sel
    idx = np.nonzero(keep)[0]
    if idx.size == 0:
        return empty
    a = 2.0 * np.pi * idx / pixel + np.radians(ROTATE_DEG if rot_deg is None else rot_deg)
    r = r_m[idx]
    return (-r * np.cos(a), -r * np.sin(a), amp[idx])  # +Y = -cos, +Z = -sin (manual)


# device status bits (lrcctrl_if.h STATUS_*)
STATUS_BITS = [
    (1 << 0, "ID"), (1 << 1, "STREAMING"), (1 << 2, "SCANNING"), (1 << 3, "ERROR"),
    (1 << 4, "WRITING"), (1 << 5, "PIN-PROTECTED"), (1 << 6, "LOCKED/LOGIN"),
    (1 << 7, "OUT-OF-SPACE"), (1 << 8, "NOT-READY/BOOTING"), (1 << 9, "BATCH"),
    (1 << 10, "PAUSE"), (1 << 11, "DATA-INVALID"), (1 << 14, "POST-PROC"),
    (1 << 15, "STARTUP"), (1 << 16, "USER-ABORT"),
]


def decode_status(v):
    on = [name for bit, name in STATUS_BITS if v & bit]
    return f"0x{v:x} [{', '.join(on) if on else 'none'}]"


def recv_exact(sock, n):
    """Read exactly n bytes or raise."""
    parts = []
    got = 0
    while got < n:
        chunk = sock.recv(n - got)
        if not chunk:
            raise ConnectionError("stream socket closed by peer")
        parts.append(chunk)
        got += len(chunk)
    return b"".join(parts)


def read_frame(sock):
    """Resync to the 'Z+F\0' marker, read the 14-byte header + payload.
    Hardened: an implausible size field (device-side desync, or a false
    marker inside pixel data) is rejected and scanning resumes at the next
    marker - a naive read would swallow SECONDS of stream as one bogus
    payload (seen live as a spontaneous multi-second outage)."""
    MAX_FRAME_BYTES = 4 << 20            # legit frames are < ~1 MiB
    while True:
        window = recv_exact(sock, 4)
        skipped = 0
        while window != MARKER:
            window = window[1:] + recv_exact(sock, 1)
            skipped += 1
        if skipped:
            print(f"# stream resync: {skipped} bytes skipped to next frame marker",
                  file=sys.stderr)
        hdr = recv_exact(sock, 10)
        ptype, size, elems = struct.unpack("<HII", hdr)
        if size > MAX_FRAME_BYTES:
            print(f"# stream desync: implausible frame size {size} (type {ptype})"
                  " - resyncing", file=sys.stderr)
            continue
        payload = recv_exact(sock, size) if size else b""
        return ptype, elems, payload


def send_cmd(cmd_sock, text, wait=2.5):
    """Send a UTF-8 CRLF-terminated command; return reply (to </xml> or timeout)."""
    cmd_sock.sendall(text.encode("utf-8") + b"\r\n")
    cmd_sock.settimeout(wait)
    reply = b""
    try:
        while True:
            chunk = cmd_sock.recv(4096)
            if not chunk:
                break
            reply += chunk
            if b"</xml>" in reply:
                break
    except socket.timeout:
        pass
    return reply.decode("utf-8", "replace").strip()


def status_int(xml_reply):
    m = re.search(r"<status>(-?\d+)</status>", xml_reply)
    if m:
        return int(m.group(1))
    s = xml_reply.strip()
    return int(s) if s.lstrip("-").isdigit() else None


def affirmative_action(xml_reply):
    m = re.search(r'act="([^"]*)"', xml_reply)
    return m.group(1).split(",")[0].strip() if m and m.group(1) else "yes"


def report(cmd_sock, label, command, wait):
    r = send_cmd(cmd_sock, command, wait)
    s = status_int(r)
    extra = "  " + decode_status(s) if s is not None else ""
    print(f"# {label}: {r!r}{extra}")
    return r


def ensure_idle(cmd_sock, wait, tries=12):
    """Answer pending messageboxes / stop running scans until idle."""
    BUSY = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 15)
    last = ""
    for _ in range(tries):
        last = send_cmd(cmd_sock, "status", wait)
        if "<messagebox" in last:
            send_cmd(cmd_sock, affirmative_action(last), wait)
            time.sleep(0.3)
            continue
        s = status_int(last)
        if s is not None and (s & BUSY):
            send_cmd(cmd_sock, "stop", wait)
            time.sleep(0.5)
            continue
        break
    s = status_int(last)
    print(f"# status (idle): {last!r}" + (f"  {decode_status(s)}" if s is not None else ""))
    return last


def local_ip_towards(target_ip):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect((target_ip, PORT_COMMAND))
        return s.getsockname()[0]
    except OSError:
        return "127.0.0.1"
    finally:
        s.close()


def connect_command(ip, timeout=5):
    try:
        return socket.create_connection((ip, PORT_COMMAND), timeout=timeout)
    except OSError as e:
        note = ""
        if local_ip_towards(ip) == ip:
            note = ("\n   That address is THIS PC's own IP. Use the scanner's address, or run"
                    "\n   python3 acquire.py --find to discover it on the subnet.")
        sys.exit(f"!! cannot reach scanner command port {ip}:{PORT_COMMAND}  ({e}){note}")


def find_scanners(local_ip, port=PORT_COMMAND, timeout=0.3):
    import concurrent.futures
    base = ".".join(local_ip.split(".")[:3])

    def probe(i):
        ip = f"{base}.{i}"
        try:
            socket.create_connection((ip, port), timeout=timeout).close()
            return ip
        except OSError:
            return None

    with concurrent.futures.ThreadPoolExecutor(max_workers=128) as ex:
        return [r for r in ex.map(probe, range(1, 255)) if r]


def raw_probe(ip):
    """Try CRLF/LF/CR terminators against the command port, dumping raw replies."""
    for term, name in ((b"\r\n", "CRLF"), (b"\n", "LF"), (b"\r", "CR")):
        print(f"\n# --- attempt: 'version' + {name} ---")
        s = connect_command(ip)
        s.settimeout(1.5)
        banner = b""
        try:
            while True:
                c = s.recv(4096)
                if not c:
                    break
                banner += c
        except socket.timeout:
            pass
        print(f"#   banner on connect: {banner[:200]!r}" if banner else "#   banner on connect: <none>")
        try:
            s.sendall(b"version" + term)
        except OSError as e:
            print(f"#   send failed: {e}")
            s.close()
            continue
        s.settimeout(3.0)
        rep = b""
        try:
            while True:
                c = s.recv(4096)
                if not c:
                    break
                rep += c
        except socket.timeout:
            pass
        print(f"#   reply: {len(rep)} bytes: {rep[:300]!r}")
        s.close()
        if rep:
            print(f"#   >>> {name} elicited a reply - use this terminator.")
            return
    print("\n# No terminator produced a reply (port held by another client, or login needed).")


def iter_profiles(ip, seconds=30.0, mode="tcpzip", resolution="10000", quality="50",
                  min_amplitude=2000.0, decimate=1, rps=None, mhz=None, scan_args=None,
                  file=None, extra=None, local_ip=None, reply_wait=2.5, log=print,
                  rotate_deg=None, min_range=0.0):
    """Generator: connect, start a profiler scan, and yield (t, y, z, intensity)
    numpy arrays for each profile until the scan stops or `seconds` elapse. Always
    stops the scan and closes the sockets on exit. (y,z metres; intensity raw.)"""
    local = local_ip or local_ip_towards(ip)
    if scan_args is not None:
        scan_cmd = "scan " + scan_args
    else:
        fname = file or ("live_" + time.strftime("%Y%m%d-%H%M%S"))
        opts = ""
        if rps is not None:
            opts += f" {RPS_FLAG} {rps:g}"
        if mhz is not None:
            opts += f" {MHZ_FLAG} {mhz:g}"
        if extra:
            opts += " " + extra
        scan_cmd = (f"scan -profiler -resolution {resolution} -quality {quality} "
                    f"-file {fname} -path scans -filesize 250mb -linestream {mode} "
                    f"-statusstream -memorize 3 -writing 0 -ip {local}{opts}")

    cmd = connect_command(ip)
    stream = None
    started = False
    decode_reset()                                # fresh temporal state per scan
    try:
        log(f"# version: {send_cmd(cmd, 'version', reply_wait)[:70]} ...")
        ensure_idle(cmd, reply_wait)
        time.sleep(0.5)
        log(f"# scan: {scan_cmd}")
        log(f"# scan reply: {send_cmd(cmd, scan_cmd, reply_wait)!r}")
        started = True
        for _ in range(10):                       # auto-answer overwrite/messageboxes
            time.sleep(0.5)
            st = send_cmd(cmd, "status", reply_wait)
            if "<messagebox" in st:
                send_cmd(cmd, affirmative_action(st), reply_wait)
                continue
            break

        deadline_conn = time.monotonic() + 10
        while stream is None and time.monotonic() < deadline_conn:
            try:
                stream = socket.create_connection((ip, PORT_STREAM), timeout=2)
            except OSError:
                time.sleep(0.5)
        if stream is None:
            log("!! could not connect to stream port")
            return

        stream.settimeout(1.0)
        pixel = None
        cur_lh = b""
        prev_sec = None
        deadline = time.monotonic() + seconds
        while time.monotonic() < deadline:
            try:
                ptype, _, payload = read_frame(stream)
            except socket.timeout:
                continue
            except (ConnectionError, OSError):
                break
            if ptype == ZFS_HEADER_ID:
                p = parse_zfs_header_pixel(payload)
                if p:
                    pixel = p
            elif ptype == LINE_HEADER_ID:
                cur_lh = payload
            elif ptype in (RAW_PIXEL_ID, COMP_PIXEL_ID):
                data = payload
                if ptype == COMP_PIXEL_ID:
                    try:
                        d = zlib.decompressobj(15)
                        data = d.decompress(payload) + d.flush()
                    except zlib.error:
                        continue
                if pixel is None:
                    pixel = len(data) // 8
                t_s = lineheader_time_ms(cur_lh) / 1000.0
                u_arr = np.frombuffer(data, dtype="<u2")
                y, z, inten = decode_profile_yz(data, pixel, min_amplitude, decimate,
                                                rot_deg=rotate_deg, min_range=min_range,
                                                prev_sec=prev_sec)
                if u_arr.size >= 4 * pixel:
                    prev_sec = u_arr[2*pixel:2*pixel + pixel//2].astype(np.float64) * 2 * RANGE_SCALE_M
                if y.size:
                    yield (t_s, y, z, inten)
    finally:
        try:
            if started:
                send_cmd(cmd, "stop", reply_wait)
        except OSError:
            pass
        if stream:
            stream.close()
        cmd.close()


REC_MAGIC = b"ZNFREC1\n"


def record_frames_iter(path):
    """Yield (ptype, payload) frames from a --record capture file."""
    with open(path, "rb") as f:
        if f.read(len(REC_MAGIC)) != REC_MAGIC:
            sys.exit(f"!! {path}: not a ZNFREC1 capture")
        while True:
            hdr = f.read(6)
            if len(hdr) < 6:
                return
            ptype, size = struct.unpack("<HI", hdr)
            payload = f.read(size)
            if len(payload) < size:
                return
            yield ptype, payload


def replay_file(args):
    """Offline decode of a --record capture through the SAME pipeline as live."""
    lines = pts = 0
    pixel = None
    cur_lh = b""
    prev_sec_main = None
    decode_reset()
    print("# t[s]      x[m]      y[m]      z[m]   intensity")
    for ptype, payload in record_frames_iter(args.replay):
        if ptype == ZFS_HEADER_ID:
            p = parse_zfs_header_pixel(payload)
            if p:
                pixel = p
                print(f"# zfs header: pixel/line = {pixel}", file=sys.stderr)
        elif ptype == LINE_HEADER_ID:
            cur_lh = payload
        elif ptype in (RAW_PIXEL_ID, COMP_PIXEL_ID):
            data = payload
            if ptype == COMP_PIXEL_ID:
                try:
                    d = zlib.decompressobj(15)
                    data = d.decompress(payload) + d.flush()
                except zlib.error:
                    continue
            if pixel is None:
                pixel = len(data) // 8
            t_s = lineheader_time_ms(cur_lh) / 1000.0
            line_idx = lineheader_number(cur_lh)
            if line_idx < 0:
                line_idx = lines
            for (t, x, y, z, inten) in decode_line(
                    data, pixel, line_idx, t_s, args.spacing,
                    args.min_amplitude, args.print_every,
                    rot_deg=args.rotate_deg, min_range=args.min_range,
                    unwrap=not args.no_unwrap, mixpix_deg=args.mixpix_deg,
                    prev_sec=prev_sec_main):
                print(f"{t:9.3f} {x:9.3f} {y:9.3f} {z:9.3f} {inten:9.0f}")
                pts += 1
            _u = np.frombuffer(data, dtype="<u2")
            if pixel and _u.size >= 4 * pixel:
                prev_sec_main = _u[2*pixel:2*pixel + pixel//2].astype(np.float64) * 2 * RANGE_SCALE_M
            lines += 1
    print(f"# replayed {lines} lines, printed {pts} points", file=sys.stderr)
    return 0


# --------------------------------------------------------------------------- #
# main
# --------------------------------------------------------------------------- #
def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("ip", nargs="?", default=None,
                    help="the SCANNER's own IP (e.g. its web-UI address) - NOT the '-ip' value "
                         "from a scan command, which is the PC/client address")
    ap.add_argument("--seconds", type=float, default=30.0, help="acquire duration (default 30)")
    ap.add_argument("--mode", choices=("tcpzip", "tcp"), default="tcpzip",
                    help="line stream encoding (default tcpzip = zlib)")
    ap.add_argument("--resolution", default="10000",
                    help="scan resolution (default 10000, from this device's recorded command)")
    ap.add_argument("--quality", default="50",
                    help="scan quality (default 50, from this device's recorded command)")
    ap.add_argument("--scan-args", default=None,
                    help="override the entire scan command's arguments (everything after 'scan')")
    ap.add_argument("--file", default=None,
                    help="scan file name (default: unique 'live_<timestamp>' so the file never "
                         "pre-exists and the scanner never asks to overwrite)")
    ap.add_argument("--rps", type=float, default=None,
                    help=f"laser motor speed [rev/s]; adds '{RPS_FLAG} <rps>' to the scan command "
                         f"(flag name inferred - check the scan reply, override via --extra)")
    ap.add_argument("--mhz", type=float, default=None,
                    help=f"laser data rate [MHz]; adds '{MHZ_FLAG} <mhz>' to the scan command "
                         f"(flag name inferred - check the scan reply, override via --extra)")
    ap.add_argument("--extra", default=None,
                    help="extra raw flags appended verbatim to the scan command "
                         "(e.g. '-speed 100 -datarate 2')")
    ap.add_argument("--reply-wait", type=float, default=2.5,
                    help="seconds to wait for a command reply (default 2.5)")
    ap.add_argument("--find", action="store_true",
                    help="diagnostic: scan the local /24 subnet for the scanner command "
                         "port (6100) and list responders, then exit")
    ap.add_argument("--probe", action="store_true",
                    help="diagnostic: connect, print version + status + prg replies, then exit (no scan)")
    ap.add_argument("--raw", action="store_true",
                    help="diagnostic: low-level - read any connect banner and try CRLF/LF/CR "
                         "terminators, dumping raw reply bytes (use when --probe shows empty replies)")
    ap.add_argument("--cmd", default=None,
                    help="diagnostic: send this single command, print the raw reply, then exit")
    ap.add_argument("--spacing", type=float, default=0.0,
                    help="SYNTHETIC travel added to X per profile [m]; default 0 = stationary. "
                         "Real travel needs an odometer/trajectory the live stream does not "
                         "carry, so only set this for a known constant-speed move.")
    ap.add_argument("--min-amplitude", type=float, default=2000.0,
                    help="skip points below this intensity (default 2000)")
    ap.add_argument("--rotate-deg", type=float, default=None,
                    help=f"angle-zero/mount correction [deg] (default {ROTATE_DEG}, calibrated)")
    ap.add_argument("--min-range", type=float, default=0.0,
                    help="drop returns closer than this [m] (default 0)")
    ap.add_argument("--no-unwrap", action="store_true",
                    help="disable gated >6.55 m unwrapping (caps range at 6.55 m)")
    ap.add_argument("--mixpix-deg", type=float, default=MIXPIX_DEG,
                    help=f"mixed-pixel edge-trail filter angle (default {MIXPIX_DEG}; 0=off)")
    ap.add_argument("--print-every", type=int, default=2048,
                    help="print every Nth valid pixel per line to keep console readable (default 2048)")
    ap.add_argument("--local-ip", default=None, help="override local IP advertised to scanner")
    ap.add_argument("--record", default=None, metavar="FILE",
                    help="diagnostic: dump every stream frame verbatim (compressed payloads "
                         "as received) to FILE for offline analysis / --replay")
    ap.add_argument("--replay", default=None, metavar="FILE",
                    help="diagnostic: decode a --record file offline through the identical "
                         "pipeline instead of connecting to a scanner (ip ignored)")
    args = ap.parse_args()

    # ---- offline replay of a --record capture (no scanner needed) --------- #
    if args.replay is not None:
        return replay_file(args)

    # ---- subnet discovery (no scanner IP needed) ------------------------- #
    if args.find:
        local = args.local_ip or local_ip_towards(args.ip or "192.168.0.1")
        base = ".".join(local.split(".")[:3])
        print(f"# this PC = {local}; scanning {base}.0/24 for command port {PORT_COMMAND} ...")
        hits = [h for h in find_scanners(local) if h != local]
        print(f"# scanner candidates (port {PORT_COMMAND} open): "
              f"{', '.join(hits) if hits else 'none found'}")
        return 0

    if not args.ip:
        ap.error("scanner IP required (or use --find to discover it on the subnet)")

    if args.raw:
        raw_probe(args.ip)
        return 0

    # ---- diagnostic modes (no scan) -------------------------------------- #
    if args.cmd is not None:
        cmd = connect_command(args.ip)
        try:
            report(cmd, f"cmd {args.cmd!r}", args.cmd, args.reply_wait)
        finally:
            cmd.close()
        return 0
    if args.probe:
        cmd = connect_command(args.ip)
        try:
            for c in ("version", "status", "prg", "help"):
                report(cmd, c, c, args.reply_wait)
        finally:
            cmd.close()
        return 0

    local = args.local_ip or local_ip_towards(args.ip)
    if args.scan_args is not None:
        scan_cmd = "scan " + args.scan_args
    else:
        # Mirrors THIS device's recorded working command (file metadata: numeric
        # resolution/quality, -path scans, -memorize 3, -statusstream), adding
        # -linestream <mode> for TCP streaming and -writing 0 (no disk write).
        # A unique file name avoids the "file exists, overwrite?" prompt entirely.
        fname = args.file or ("live_" + time.strftime("%Y%m%d-%H%M%S"))
        opts = ""
        if args.rps is not None:
            opts += f" {RPS_FLAG} {args.rps:g}"
        if args.mhz is not None:
            opts += f" {MHZ_FLAG} {args.mhz:g}"
        if args.extra:
            opts += " " + args.extra
        scan_cmd = (f"scan -profiler -resolution {args.resolution} -quality {args.quality} "
                    f"-file {fname} -path scans -filesize 250mb -linestream {args.mode} "
                    f"-statusstream -memorize 3 -writing 0 -ip {local}{opts}")

    print(f"# connecting to scanner {args.ip} (command :{PORT_COMMAND}, stream :{PORT_STREAM})")
    print(f"# local ip advertised: {local}")

    cmd = connect_command(args.ip)
    stream = None
    started = False
    lines = pts = 0
    pixel = None
    rec_fp = None
    if args.record:
        rec_fp = open(args.record, "wb")
        rec_fp.write(REC_MAGIC)
        print(f"# recording every stream frame to {args.record}", file=sys.stderr)
    decode_reset()                                # fresh temporal state per scan
    try:
        report(cmd, "version", "version", args.reply_wait)
        ensure_idle(cmd, args.reply_wait)         # clear leftover box / stop any running scan
        time.sleep(0.5)

        print(f"# scan command: {scan_cmd}")
        report(cmd, "scan reply", scan_cmd, args.reply_wait)
        started = True

        # The device may pop a yes/no messagebox (e.g. "File already exists!
        # Overwrite?") and block until answered. Poll status and auto-answer any
        # messagebox with its affirmative action ("yes") so it runs unattended.
        for _ in range(10):
            time.sleep(0.5)
            st = send_cmd(cmd, "status", args.reply_wait)
            if "<messagebox" in st:
                ans = affirmative_action(st)
                print(f"# messagebox: {st!r}")
                print(f"#   -> auto-answering {ans!r}")
                send_cmd(cmd, ans, args.reply_wait)
                continue
            s = status_int(st)
            print(f"# status (after): {st!r}" + (f"  {decode_status(s)}" if s is not None else ""))
            break

        # connect the stream socket (the server starts pushing once the scan runs)
        deadline_conn = time.monotonic() + 10
        while stream is None and time.monotonic() < deadline_conn:
            try:
                stream = socket.create_connection((args.ip, PORT_STREAM), timeout=2)
            except OSError:
                time.sleep(0.5)
        if stream is None:
            print("!! could not connect to stream port; aborting", file=sys.stderr)
            return 1

        print("# t[s]      x[m]      y[m]      z[m]   intensity")
        prev_sec_main = None
        stream.settimeout(1.0)
        deadline = time.monotonic() + args.seconds
        last_stat = time.monotonic()
        cur_lh = b""                              # most recent LINE_HEADER payload
        while time.monotonic() < deadline:
            try:
                ptype, elems, payload = read_frame(stream)
            except socket.timeout:
                continue
            except (ConnectionError, OSError) as e:
                print(f"!! stream error: {e}", file=sys.stderr)
                break

            if rec_fp:
                rec_fp.write(struct.pack("<HI", ptype, len(payload)) + payload)

            if ptype == ZFS_HEADER_ID:
                p = parse_zfs_header_pixel(payload)
                if p:
                    pixel = p
                    print(f"# zfs header: pixel/line = {pixel}")
            elif ptype == LINE_HEADER_ID:
                cur_lh = payload                  # keep for the following pixel msg
            elif ptype in (RAW_PIXEL_ID, COMP_PIXEL_ID):
                data = payload
                if ptype == COMP_PIXEL_ID:
                    try:
                        d = zlib.decompressobj(15)
                        data = d.decompress(payload) + d.flush()
                    except zlib.error:
                        continue
                if pixel is None:                 # fall back: infer pixel (8 B/pixel)
                    pixel = len(data) // 8
                t_s = lineheader_time_ms(cur_lh) / 1000.0
                line_idx = lineheader_number(cur_lh)
                if line_idx < 0:
                    line_idx = lines
                for (t, x, y, z, inten) in decode_line(
                        data, pixel, line_idx, t_s, args.spacing,
                        args.min_amplitude, args.print_every,
                        rot_deg=args.rotate_deg, min_range=args.min_range,
                        unwrap=not args.no_unwrap, mixpix_deg=args.mixpix_deg,
                        prev_sec=prev_sec_main):
                    print(f"{t:9.3f} {x:9.3f} {y:9.3f} {z:9.3f} {inten:9.0f}")
                    pts += 1
                _u = np.frombuffer(data, dtype="<u2")
                if _u.size >= 4 * pixel:
                    prev_sec_main = _u[2*pixel:2*pixel + pixel//2].astype(np.float64) * 2 * RANGE_SCALE_M
                lines += 1

            now = time.monotonic()
            if now - last_stat >= 1.0:
                print(f"# ... {lines} lines, {pts} points printed", file=sys.stderr)
                last_stat = now

        print(f"# acquired ~{lines} lines, printed {pts} points", file=sys.stderr)
        return 0
    finally:
        # always stop the scan and clean up
        try:
            if started:
                print(f"# stop reply: {send_cmd(cmd, 'stop')!r}", file=sys.stderr)
        except OSError:
            pass
        if stream:
            stream.close()
        if rec_fp:
            rec_fp.close()
        cmd.close()


if __name__ == "__main__":
    sys.exit(main())
