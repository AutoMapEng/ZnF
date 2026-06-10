#!/usr/bin/env python3
"""
zfs2ply_ascii.py - Convert a Z+F PROFILER (.zfs) scan file to an ASCII PLY cloud.

This is the ASCII variant of zfs2ply.py: the vertex data is written as
space-delimited ASCII text (PLY "format ascii 1.0") instead of binary.

Usage:
    python3 zfs2ply_ascii.py INPUT.zfs [-o OUTPUT.ply] [options]

Output PLY vertex fields:  x, y, z (float), intensity (float), timestamp (double)
Each vertex is one line:    "<x> <y> <z> <intensity> <timestamp>"

------------------------------------------------------------------------------
.zfs container format (reverse-engineered):
  * 8-byte file header: b"ZFS" + version + 2x uint16
  * TLV metadata header (records end at tag 0xFFFF), then 0xFF padding to 0x8000
  * data section @ 0x8000: a sequence of blocks (one per scan profile), each:
        uint32  length        bytes following this field (block-content size)
        bytes   "AA 55 34 12"  sync magic
        0x30 B  block header   (per-profile: +0x20 uint32 timestamp, +0x24 index)
        zlib    deflate stream (CMF/FLG 78 01), Z_SYNC_FLUSH-terminated
    zlib data slice = block[0x38 : 0x04+length]
    each profile = points/2 DIRECTIONS measured twice (dual modulation):
    fine = plane0[half+j] (0.1 mm, 6.5536 m ambiguity), fine amp = plane1[half+j],
    coarse = plane2[j] (0.2 mm) resolves the wrap; fine < 0.345 m = no return

Geometry: the PROFILER is a KINEMATIC (mobile) line scanner - each profile is a
2D cross-section (the mirror sweeps a full vertical circle) taken as the unit
travels forward. Profiles are stacked by LINEAR TRANSLATION along the travel
axis, NOT by rotating an azimuth (that yields a surface of revolution -
concentric arcs - which is wrong). Axes match the Z+F SDK profiler convention:
X = travel (forward), (Y,Z) = the cross-section plane.

    alpha = 2*pi * point/points_per_profile * (vertical_deg/360)   # in-profile angle
    r     = range_raw * scale                                      # scale = 1/10 mm (SDK)
    mode=translate (default):  x = (profile + frac)*spacing  [helix];
                               y = -r*cos(alpha); z = -r*sin(alpha)
    mode=rotate (dome scans):  x = r*cos(alpha)*cos(phi); y = r*cos(alpha)*sin(phi);
                               z = -r*sin(alpha);  phi = radians(azimuth_deg)*profile/n
  helix: within one rotation the platform advances, so each pixel's travel gets a
         (point/points)*spacing offset (Z+F WayCounting::CalcWayWithHelix).

NOTE: `scale` 0.0001 = 1/10 mm is the Z+F SDK raw range unit; exact metric range
additionally applies the on-device MCAL calibration polynomial (compiled in the
SDK, not reproduced here). `spacing` (metres of travel per profile) is NOT stored
in the file - true travel needs the platform trajectory (odometer/GPS/IMU).
------------------------------------------------------------------------------
"""
import sys
import os
import struct
import zlib
import argparse

try:
    import numpy as np
except ImportError:
    sys.exit("This script requires numpy:  pip install numpy")

MAGIC       = b"ZFS"
DATA_START  = 0x8000          # data section begins here (header is padded to 32 KB)
SYNC        = b"\xaa\x55\x34\x12"
ZLIB_OFF    = 0x38            # zlib stream offset within each block
HDR_TS_OFF  = 0x20           # uint32 per-profile device timestamp (~ms)
HDR_IDX_OFF = 0x24           # uint32 profile index


def find_points_per_profile(d, default=10240):
    """Scan the TLV metadata header for tag 0x0002 (samples per profile)."""
    pos = 8
    try:
        while pos + 4 <= DATA_START:
            tag    = struct.unpack_from("<H", d, pos)[0]
            length = d[pos + 2]
            typ    = d[pos + 3]
            if tag == 0xFFFF:
                break
            if typ == 0x01:                                   # string record
                pos += 16 + ((length + 15) // 16) * 16
                continue
            if typ == 0x00 and length in (1, 2, 4, 8, 9, 12, 16):
                if tag == 0x0002 and length == 4:
                    return struct.unpack_from("<I", d, pos + 4)[0]
                pos += ((4 + length + 15) // 16) * 16
                continue
            break                                             # unknown -> stop
    except struct.error:
        pass
    return default


def iter_blocks(d):
    """Yield (offset, length) for every data block."""
    pos, n = DATA_START, len(d)
    while pos + 8 <= n and d[pos + 4:pos + 8] == SYNC:
        length = struct.unpack_from("<I", d, pos)[0]
        yield pos, length
        pos += 4 + length


def decompress_block(d, pos, length):
    """Inflate one profile's zlib stream (bounded; sync-flush terminated)."""
    o = zlib.decompressobj(15)
    return o.decompress(d[pos + ZLIB_OFF: pos + 4 + length]) + o.flush()


def write_ply_ascii(path, x, y, z, inten, tstamp, meta, chunk=1_000_000):
    """Write a space-delimited ASCII PLY (streamed in chunks to bound memory)."""
    npts = x.size
    header = (
        "ply\n"
        "format ascii 1.0\n"
        "comment Generated by zfs2ply_ascii.py from a Z+F PROFILER .zfs scan\n"
        f"comment grid: {meta['profiles']} profiles x {meta['points'] // 2} directions "
        f"(dual-modulation decode)\n"
        f"comment range scale: {meta['scale']} metres/unit (1/10 mm, Z+F SDK)\n"
        f"comment stacking: {meta['mode']} ({meta['stack']}, provisional)\n"
        "comment timestamp: device clock (~ms), interpolated per point\n"
        f"element vertex {npts}\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "property float intensity\n"
        "property double timestamp\n"
        "end_header\n"
    )
    col_fmt = ("%.6f", "%.6f", "%.6f", "%.1f", "%.3f")
    with open(path, "w") as f:
        f.write(header)
        for s in range(0, npts, chunk):
            e = min(s + chunk, npts)
            block = np.empty((e - s, 5), dtype=np.float64)
            block[:, 0] = x[s:e]
            block[:, 1] = y[s:e]
            block[:, 2] = z[s:e]
            block[:, 3] = inten[s:e]
            block[:, 4] = tstamp[s:e]
            np.savetxt(f, block, fmt=col_fmt, delimiter=" ")


def main():
    ap = argparse.ArgumentParser(
        description="Convert a Z+F .zfs scan to an ASCII PLY point cloud "
                    "(x, y, z, intensity, timestamp).")
    ap.add_argument("input", help="input .zfs file")
    ap.add_argument("-o", "--output",
                    help="output .ply (default: input name + _ascii.ply)")
    ap.add_argument("--mode", choices=("translate", "rotate"), default="translate",
                    help="profile stacking: 'translate' for a mobile line profiler "
                         "(default), 'rotate' for a static dome scan")
    ap.add_argument("--scale", type=float, default=0.0001,
                    help="metres per raw range unit (default 0.0001 = 1/10 mm per "
                         "Z+F SDK; exact range also applies MCAL calibration)")
    ap.add_argument("--spacing", type=float, default=0.05,
                    help="[translate] metres of travel per profile (provisional, "
                         "default 0.05; real travel needs the platform trajectory)")
    ap.add_argument("--no-helix", action="store_true",
                    help="[translate] disable the intra-profile helix travel offset "
                         "(the platform moves during each rotation; on by default)")
    ap.add_argument("--azimuth-deg", type=float, default=180.0,
                    help="[rotate] head rotation span across all profiles (default 180)")
    ap.add_argument("--vertical-deg", type=float, default=360.0,
                    help="mirror sweep span per profile (default 360)")
    ap.add_argument("--no-unwrap", action="store_true",
                    help="disable gated >6.55 m unwrapping (caps range at 6.55 m)")
    ap.add_argument("--mixpix-deg", type=float, default=8.0,
                    help="mixed-pixel edge-trail filter angle (default 8.0; 0=off)")
    ap.add_argument("--rotate-deg", type=float, default=90.8,
                    help="angle-zero/mount correction [deg] (default 90.8, calibrated)")
    ap.add_argument("--min-amplitude", type=float, default=1.0,
                    help="drop points whose intensity is below this "
                         "(default 1 = keep all real returns; raise to denoise)")
    ap.add_argument("--max-range", type=float, default=0.0,
                    help="drop points beyond this range in metres (0 = no limit)")
    args = ap.parse_args()

    out_path = args.output or (os.path.splitext(args.input)[0] + "_ascii.ply")

    with open(args.input, "rb") as f:
        d = f.read()
    if d[:3] != MAGIC:
        sys.exit(f"{args.input}: not a ZFS file (magic={d[:4]!r})")

    points = find_points_per_profile(d)
    blocks = list(iter_blocks(d))
    if not blocks:
        sys.exit(f"{args.input}: no data blocks found at 0x{DATA_START:x}")
    n_prof = len(blocks)

    # per-profile device timestamps -> per-profile delta (for interpolation)
    ts = np.array([struct.unpack_from("<I", d, p + HDR_TS_OFF)[0] for p, _ in blocks],
                  dtype=np.float64)
    dt = np.diff(ts)
    dt = np.append(dt, dt[-1] if dt.size else 0.0)

    # learn channel count from the first profile
    raw0 = decompress_block(d, *blocks[0])
    total_u16 = len(raw0) // 2
    if total_u16 == 0 or total_u16 % points != 0:
        sys.exit(f"unexpected profile size {len(raw0)} B for {points} points/profile")
    channels = total_u16 // points
    dirs = points // 2                       # directions per revolution (dual-mod)

    # precompute per-direction angle terms (points//2 directions per revolution)
    dirs = points // 2
    pt = np.arange(dirs)
    alpha = (2.0 * np.pi * (pt / dirs) * (args.vertical_deg / 360.0)
             + np.radians(args.rotate_deg))
    cos_a, sin_a = np.cos(alpha), np.sin(alpha)
    frac = pt / dirs                         # within-profile fraction (time & helix)
    azi = np.radians(args.azimuth_deg)       # rotate mode only
    helix = not args.no_helix                # intra-profile travel ramp (translate mode)

    xs, ys, zs, ii, tt = [], [], [], [], []
    prev_sec = None
    for k, (p, length) in enumerate(blocks):
        raw = raw0 if k == 0 else decompress_block(d, p, length)
        u = np.frombuffer(raw, dtype="<u2")
        if u.size != total_u16:              # ragged/truncated block -> skip
            continue
        # dual-modulation layout (validated 2026-06-10): points//2 directions;
        # fine = plane0[half+j] (scale units, 6.5536 m ambiguity), fine amp =
        # plane1[half+j], coarse = plane2[j] (2x scale) resolves the wrap.
        half = points // 2
        fine = u[half:points].astype(np.float64) * args.scale
        amp = u[points + half:2 * points].astype(np.float64)
        sec = u[2 * points:2 * points + half].astype(np.float64) * 2 * args.scale
        fraw = u[half:points].astype(np.int64)
        sraw = u[2 * points:2 * points + half].astype(np.int64)
        code = ((fraw & 0xFF) == (fraw >> 8)) & (sraw == fraw)   # status, not range
        kw = np.clip(np.round((sec - fine) / 6.5536), 0, 2)
        if args.no_unwrap:
            kw = np.zeros_like(kw)
        rng = fine + kw * 6.5536
        bad = code | ((kw == 0) & (fine < 0.345))
        bad |= np.abs(sec - rng) > 2.5       # scales disagree: mixed echo
        bad |= rng > 200.0                   # spec max-range guard
        rng = np.where(bad, 0.0, rng)

        m = (amp >= args.min_amplitude) & (rng > 0)   # rng==0 = no-return sentinel
        # mixed-pixel (edge-trail) filter: drop points whose local chord is
        # nearly parallel to the beam (phase scanners bridge depth edges)
        if args.mixpix_deg > 0:
            W = 4
            aa = alpha
            py = -rng * cos_a; pz = -rng * sin_a
            vful = m.copy()
            cy = py[2*W:] - py[:-2*W]; cz = pz[2*W:] - pz[:-2*W]
            cn = np.hypot(cy, cz)
            dot = np.abs(cy * (-cos_a[W:-W]) + cz * (-sin_a[W:-W]))
            with np.errstate(invalid="ignore", divide="ignore"):
                gdeg = np.degrees(np.arccos(np.clip(dot / np.where(cn > 1e-9, cn, np.inf), 0, 1)))
            okn = vful[W:-W] & vful[2*W:] & vful[:-2*W]
            dropm = np.zeros(dirs, bool)
            dropm[W:-W] = okn & (gdeg < args.mixpix_deg)
            nb = np.convolve(dropm.astype(np.int8), np.array([1,1,0,1,1],np.int8), mode="same")
            dropm |= vful & (nb >= 2)
            m &= ~dropm

        if args.max_range > 0:
            m &= rng <= args.max_range
        if not m.any():
            continue

        r = rng[m]
        ca_m, sa_m = cos_a[m], sin_a[m]
        if args.mode == "rotate":                 # static dome scan
            phi = azi * (k / n_prof)
            rc_a = -(r * ca_m)                    # +Y/-radial = -cos (manual frame)
            xs.append((rc_a * np.cos(phi)).astype(np.float32))
            ys.append((rc_a * np.sin(phi)).astype(np.float32))
            zs.append((-(r * sa_m)).astype(np.float32))   # +Z = -sin (Z+F manual)
        else:                                     # translate: mobile line profiler
            # Z+F convention: X = travel (forward), (Y,Z) = cross-section plane.
            # helix: platform advances during each rotation -> add frac*spacing.
            if helix:
                travel = (k + frac[m]) * args.spacing
            else:
                travel = np.full(r.shape, k * args.spacing)
            xs.append(travel.astype(np.float32))                 # travel (forward)
            ys.append((-(r * ca_m)).astype(np.float32))          # cross-section horizontal (+Y = -cos, manual)
            zs.append((-(r * sa_m)).astype(np.float32))          # cross-section vertical (+Z = -sin, manual)
        ii.append(amp[m].astype(np.float32))
        tt.append((ts[k] + frac[m] * dt[k]).astype(np.float64))

    if not xs:
        sys.exit("no valid points after filtering")

    x = np.concatenate(xs); y = np.concatenate(ys); z = np.concatenate(zs)
    inten = np.concatenate(ii); tstamp = np.concatenate(tt)

    meta = dict(profiles=n_prof, points=points, channels=channels, scale=args.scale,
                mode=args.mode,
                stack=(f"spacing={args.spacing} m/profile, helix="
                       f"{'off' if args.no_helix else 'on'}" if args.mode == "translate"
                       else f"azimuth={args.azimuth_deg} deg"))
    write_ply_ascii(out_path, x, y, z, inten, tstamp, meta)

    print(f"{args.input}: {n_prof} profiles x {points // 2} directions (dual-modulation)")
    print(f"timestamp span: {ts[0]:.0f} .. {ts[-1]:.0f} "
          f"({(ts[-1] - ts[0]) / 1000.0:.2f} s @ "
          f"~{n_prof / max((ts[-1]-ts[0])/1000.0, 1e-9):.0f} profiles/s)")
    print(f"wrote {out_path}: {x.size:,} points (ascii)")


if __name__ == "__main__":
    main()
