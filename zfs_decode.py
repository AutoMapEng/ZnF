#!/usr/bin/env python3
"""Decoder for Z+F PROFILER .zfs scan files (reverse-engineered).
Layout:
  [8B file header "ZFS"\x01 ..][TLV metadata header, ends tag 0xFFFF][0xFF pad to 0x8000]
  then N data blocks, each:
     u32 length  | 'aa 55 34 12' sync | 0x30B block hdr (timestamps) | zlib(78 01) stream
     length = bytes following the length field; zlib data = [block+0x38 : block+4+length]
     each zlib stream is Z_SYNC_FLUSH-terminated and decompresses to 81920 bytes
     = 10240 points x 4 planar uint16 channels [range/phase, amplitude, ch2, ch3]
"""
import struct, zlib, sys, numpy as np

def decode(path):
    d = open(path, "rb").read()
    assert d[:3] == b"ZFS"
    pos = 0x8000
    profs = []
    while pos + 8 <= len(d) and d[pos+4:pos+8] == b"\xaa\x55\x34\x12":
        length = struct.unpack_from("<I", d, pos)[0]
        z = d[pos+0x38 : pos+4+length]
        out = zlib.decompressobj(15)
        raw = out.decompress(z) + out.flush()
        assert len(raw) == 81920, len(raw)
        profs.append(np.frombuffer(raw, dtype="<u2"))
        pos += 4 + length
    arr = np.array(profs)                      # (N, 40960) u16
    N = arr.shape[0]
    ch = arr.reshape(N, 4, 10240)              # (N, channel, point)
    return ch                                  # ch[:,0]=range, ch[:,1]=amplitude, ...

if __name__ == "__main__":
    ch = decode(sys.argv[1] if len(sys.argv) > 1 else "test2.zfs")
    print("profiles:", ch.shape[0], " channels:", ch.shape[1], " points/profile:", ch.shape[2])
    for i in range(ch.shape[1]):
        c = ch[:, i]
        print(f"  ch{i}: min={c.min()} max={c.max()} mean={c.mean():.0f} zero%={ (c==0).mean()*100:.0f}")
