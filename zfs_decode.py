#!/usr/bin/env python3
"""Decoder for Z+F PROFILER .zfs scan files (reverse-engineered, validated).
Layout:
  [8B file header "ZFS"\x01 ..][TLV metadata header, ends tag 0xFFFF][0xFF pad to 0x8000]
  then N data blocks (one per revolution), each:
     u32 length | 'aa 55 34 12' sync | 0x30B line header | zlib(78 01) stream
     zlib data = [block+0x38 : block+4+length], Z_SYNC_FLUSH-terminated,
     inflates to 81920 B = 4 x 10240 u16 planes.
Dual-modulation content (one revolution = 5120 DIRECTIONS, each measured twice):
     fine range   = plane0[5120+j] * 0.1 mm   (ambiguity interval 6.5536 m)
     fine amp     = plane1[5120+j]
     coarse range = plane2[j]      * 0.2 mm   (resolves the ambiguity)
     true r = fine + 6.5536 * clip(round((coarse - fine)/6.5536), 0, 5)
     fine < 0.345 m = no-return sentinel (r set to 0)
"""
import struct, zlib, sys, numpy as np

AMBIG_M = 6.5536
SENTINEL_MIN_R = 0.345


def decode(path):
    """Return (r, amp): float arrays of shape (n_profiles, 5120);
    r in metres (0 = no return), amp = raw fine amplitude."""
    d = open(path, "rb").read()
    assert d[:3] == b"ZFS"
    pos = 0x8000
    rs, amps = [], []
    prev_sec = None
    while pos + 8 <= len(d) and d[pos+4:pos+8] == b"\xaa\x55\x34\x12":
        length = struct.unpack_from("<I", d, pos)[0]
        o = zlib.decompressobj(15)
        raw = o.decompress(d[pos+0x38 : pos+4+length]) + o.flush()
        assert len(raw) == 81920, len(raw)
        u = np.frombuffer(raw, dtype="<u2")
        fine = u[5120:10240].astype(np.float64) * 1e-4
        amp = u[10240+5120:20480].astype(np.float64)
        coarse = u[20480:20480+5120].astype(np.float64) * 2e-4
        fraw = u[5120:10240].astype(np.int64); sraw = u[20480:20480+5120].astype(np.int64)
        code = ((fraw & 0xFF) == (fraw >> 8)) & (sraw == fraw)   # status, not range
        k = np.clip(np.round((coarse - fine) / AMBIG_M), 0, 2)
        r = fine + k * AMBIG_M
        bad = code | ((k == 0) & (fine < SENTINEL_MIN_R))
        bad |= np.abs(coarse - r) > 2.5
        bad |= r > 200.0
        r = np.where(bad, 0.0, r)
        rs.append(r)
        amps.append(amp)
        pos += 4 + length
    return np.array(rs), np.array(amps)


if __name__ == "__main__":
    r, amp = decode(sys.argv[1] if len(sys.argv) > 1 else "test2.zfs")
    v = r > 0
    print(f"profiles: {r.shape[0]}  directions/profile: {r.shape[1]}")
    print(f"valid returns: {v.mean()*100:.0f}%   r[{r[v].min():.2f},{r[v].max():.2f}] m"
          f"   amp max {amp.max():.0f}")
