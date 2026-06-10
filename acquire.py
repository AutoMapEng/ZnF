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
    cross-section  Y = r*cos a ,  Z = r*sin a   ;  X = line * spacing (synthetic; 0=stationary)
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

# Scan-command flags for motor speed / laser data-rate. These are parsed by the
# DEVICE firmware (not exposed in the SDK headers, so the names are inferred).
# If the scan reply shows an error, fix the token here or use --extra / --scan-args.
RPS_FLAG = "-speed"          # laser motor speed   [rev/s]
MHZ_FLAG = "-datarate"       # laser data rate     [MHz]

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


# --------------------------------------------------------------------------- #
# socket helpers
# --------------------------------------------------------------------------- #
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
    """Resync to the 'Z+F\\0' marker, read the 14-byte header + payload.
    Returns (payload_type, elems, payload_bytes)."""
    window = recv_exact(sock, 4)
    while window != MARKER:                       # byte-wise resync
        window = window[1:] + recv_exact(sock, 1)
    hdr = recv_exact(sock, 10)
    ptype, size, elems = struct.unpack("<HII", hdr)
    payload = recv_exact(sock, size) if size else b""
    return ptype, elems, payload


def send_cmd(cmd_sock, text, wait=2.5):
    """Send a UTF-8 command (CRLF-terminated - confirmed against the device) and
    return the reply. Returns as soon as a complete XML reply (</xml>) arrives,
    otherwise after `wait` seconds."""
    cmd_sock.sendall(text.encode("utf-8") + b"\r\n")
    cmd_sock.settimeout(wait)
    reply = b""
    try:
        while True:
            chunk = cmd_sock.recv(4096)
            if not chunk:
                break
            reply += chunk
            if b"</xml>" in reply:           # full reply received -> return now
                break
    except socket.timeout:
        pass
    return reply.decode("utf-8", "replace").strip()


def status_int(xml_reply):
    """Extract the integer from a <status>N</status> reply (or a bare int)."""
    m = re.search(r"<status>(-?\d+)</status>", xml_reply)
    if m:
        return int(m.group(1))
    s = xml_reply.strip()
    return int(s) if s.lstrip("-").isdigit() else None


def affirmative_action(xml_reply):
    """First action keyword of a messagebox act="yes,no" (-> 'yes')."""
    m = re.search(r'act="([^"]*)"', xml_reply)
    return m.group(1).split(",")[0].strip() if m and m.group(1) else "yes"


def report(cmd_sock, label, command, wait):
    """Send a command and print the reply, decoding the status word if present."""
    r = send_cmd(cmd_sock, command, wait)
    s = status_int(r)
    extra = "  " + decode_status(s) if s is not None else ""
    print(f"# {label}: {r!r}{extra}")
    return r


def ensure_idle(cmd_sock, wait, tries=12):
    """Bring the device to an idle, ready state: auto-answer any pending
    messagebox (e.g. a leftover 'Overwrite?' box) and stop any running/stuck
    scan from a previous run. Returns the final status reply."""
    BUSY = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 15)   # STREAMING|SCANNING|ERROR|STARTUP
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
    """Best-effort local IP that routes to the scanner (for -ip)."""
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect((target_ip, PORT_COMMAND))
        return s.getsockname()[0]
    except OSError:
        return "127.0.0.1"
    finally:
        s.close()


def connect_command(ip, timeout=5):
    """Connect to the scanner command port (6100) with a clear error on failure."""
    try:
        return socket.create_connection((ip, PORT_COMMAND), timeout=timeout)
    except OSError as e:
        note = ""
        if local_ip_towards(ip) == ip:
            note = ("\n   That address is THIS PC's own IP. In a Z+F scan command the '-ip'"
                    "\n   value is the PC/client address (where the scanner streams TO), not the"
                    "\n   scanner's address. Use the scanner's own IP (its web-UI address), or"
                    "\n   run:  python3 acquire.py --find   to discover it on the subnet.")
        sys.exit(f"!! cannot reach scanner command port {ip}:{PORT_COMMAND}  ({e}){note}")


def find_scanners(local_ip, port=PORT_COMMAND, timeout=0.3):
    """Scan the local /24 for hosts whose command port is open (candidate scanners)."""
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
    """Low-level command-channel diagnosis: read any connect banner, then try
    each line terminator and dump the raw bytes the device sends back."""
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
        rep, closed = b"", False
        try:
            while True:
                c = s.recv(4096)
                if not c:
                    closed = True
                    break
                rep += c
        except socket.timeout:
            pass
        state = " (device closed connection)" if closed else " (silent / timeout)"
        print(f"#   reply: {len(rep)} bytes{state if not rep else ''}: {rep[:300]!r}")
        s.close()
        if rep:
            print(f"#   >>> {name} elicited a reply - use this terminator.")
            return
    print("\n# No terminator produced a reply. Most likely another client holds the")
    print("# command port (close the scanner's web UI / LaserControl and retry), or")
    print("# the device closed the socket (see above) and needs login/PIN first.")


# --------------------------------------------------------------------------- #
# decoding
# --------------------------------------------------------------------------- #
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


def decode_line(pixdata, pixel, line_idx, t_s, spacing, min_amp, every):
    """Decompressed pixel block -> list of (t,x,y,z,intensity) for kept points.
    Planar channels: ch0=range, ch1=intensity (validated against .zfs)."""
    u = np.frombuffer(pixdata, dtype="<u2")
    if pixel <= 0 or u.size < 2 * pixel:
        return []
    rng = u[0:pixel].astype(np.float64)
    amp = u[pixel:2 * pixel].astype(np.float64)
    i = np.arange(pixel)
    keep = (amp > min_amp) & (rng > 0)
    if every > 1:
        sel = np.zeros(pixel, bool)
        sel[::every] = True
        keep &= sel
    idx = np.nonzero(keep)[0]
    if idx.size == 0:
        return []
    a = 2.0 * np.pi * idx / pixel
    r = rng[idx] * RANGE_SCALE_M
    y = r * np.cos(a)
    z = r * np.sin(a)
    x = float(line_idx) * spacing            # synthetic travel; 0 (default) = stationary
    return [(t_s, x, float(yy), float(zz), float(aa))
            for yy, zz, aa in zip(y, z, amp[idx])]


def decode_profile_yz(pixdata, pixel, min_amp, decimate=1):
    """One profile's pixel data -> (y, z, intensity) numpy arrays (cross-section,
    metres). Same model as decode_line but returns arrays for plotting."""
    u = np.frombuffer(pixdata, dtype="<u2")
    empty = (np.empty(0), np.empty(0), np.empty(0))
    if pixel <= 0 or u.size < 2 * pixel:
        return empty
    rng = u[0:pixel].astype(np.float64)
    amp = u[pixel:2 * pixel].astype(np.float64)
    keep = (amp > min_amp) & (rng > 0)
    if decimate > 1:
        sel = np.zeros(pixel, bool)
        sel[::decimate] = True
        keep &= sel
    idx = np.nonzero(keep)[0]
    if idx.size == 0:
        return empty
    a = 2.0 * np.pi * idx / pixel
    r = rng[idx] * RANGE_SCALE_M
    return (r * np.cos(a), r * np.sin(a), amp[idx])


def iter_profiles(ip, seconds=30.0, mode="tcpzip", resolution="10000", quality="50",
                  min_amplitude=2000.0, decimate=1, rps=None, mhz=None, scan_args=None,
                  file=None, extra=None, local_ip=None, reply_wait=2.5, log=print):
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
                y, z, inten = decode_profile_yz(data, pixel, min_amplitude, decimate)
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
    ap.add_argument("--print-every", type=int, default=2048,
                    help="print every Nth valid pixel per line to keep console readable (default 2048)")
    ap.add_argument("--local-ip", default=None, help="override local IP advertised to scanner")
    args = ap.parse_args()

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
                        args.min_amplitude, args.print_every):
                    print(f"{t:9.3f} {x:9.3f} {y:9.3f} {z:9.3f} {inten:9.0f}")
                    pts += 1
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
        cmd.close()


if __name__ == "__main__":
    sys.exit(main())
