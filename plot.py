#!/usr/bin/env python3
"""
plot.py - live Y-Z cross-section plot for a Z+F PROFILER scan.

Pops up a matplotlib window and plots each profile's measured (Y, Z) points in
real time (Y = horizontal, Z = vertical, colour = intensity). The window closes
automatically when the scanner stops (after --seconds, on disconnect, or if you
close the window).

It drives the scanner via acquire.iter_profiles() (same command/stream/decode
path as acquire.py), so the device session and geometry are identical.

Usage:
    python3 plot.py 192.168.1.23
    python3 plot.py 192.168.1.23 --seconds 60 --rps 100 --mhz 2 --decimate 1
"""
import sys
import time
import argparse
from collections import deque

import numpy as np

import acquire


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("ip", help="scanner IP (e.g. 192.168.1.23)")
    ap.add_argument("--seconds", type=float, default=30.0, help="acquire duration (default 30)")
    ap.add_argument("--mode", choices=("tcpzip", "tcp"), default="tcpzip")
    ap.add_argument("--resolution", default="10000")
    ap.add_argument("--quality", default="50")
    ap.add_argument("--rps", type=float, default=None, help="laser motor speed [rev/s]")
    ap.add_argument("--mhz", type=float, default=None, help="laser data rate [MHz]")
    ap.add_argument("--min-amplitude", type=float, default=2000.0,
                    help="drop returns below this intensity (default 2000)")
    ap.add_argument("--decimate", type=int, default=2,
                    help="use every Nth pixel for speed (default 2; 1 = all points)")
    ap.add_argument("--persist", type=int, default=1,
                    help="number of most-recent profiles kept on screen (default 1)")
    ap.add_argument("--point-size", type=float, default=4.0, help="marker size (default 4)")
    ap.add_argument("--fps", type=float, default=20.0, help="max redraw rate (default 20)")
    ap.add_argument("--local-ip", default=None)
    args = ap.parse_args()

    # matplotlib is imported here so --help works without a display
    import matplotlib
    import matplotlib.pyplot as plt
    plt.ion()
    try:
        fig, ax = plt.subplots(figsize=(7.5, 7.5))
    except Exception as e:                          # e.g. TkAgg with no $DISPLAY
        sys.exit(f"Cannot open a plot window ({type(e).__name__}: {e}). Run on a desktop "
                 f"session (DISPLAY set), or use acquire.py for text output.")
    if matplotlib.get_backend().lower() == "agg":
        sys.exit("matplotlib is using the non-interactive 'Agg' backend (no display); "
                 "run on a desktop session, or use acquire.py for text output.")
    sc = ax.scatter([], [], s=args.point_size, c=[], cmap="viridis", vmin=0, vmax=60000)
    ax.set_xlabel("Y  [m]   (horizontal)")
    ax.set_ylabel("Z  [m]   (vertical)")
    ax.set_aspect("equal", "box")
    ax.grid(True, alpha=0.3)
    ax.plot(0, 0, "r+", ms=10)                      # scanner origin
    ax.set_title("Z+F live cross-section - connecting ...")
    fig.colorbar(sc, ax=ax, label="intensity")
    try:
        fig.canvas.manager.set_window_title("Z+F live Y-Z")
    except Exception:
        pass

    closed = {"v": False}
    fig.canvas.mpl_connect("close_event", lambda _e: closed.__setitem__("v", True))
    fig.show()
    plt.pause(0.01)

    buf = deque(maxlen=max(1, args.persist))
    n_prof = n_pts = 0
    ext = 0.5                                       # running half-extent for the (square) view
    min_redraw = 1.0 / max(args.fps, 1.0)
    last_draw = 0.0

    gen = acquire.iter_profiles(
        args.ip, seconds=args.seconds, mode=args.mode, resolution=args.resolution,
        quality=args.quality, min_amplitude=args.min_amplitude, decimate=args.decimate,
        rps=args.rps, mhz=args.mhz, local_ip=args.local_ip,
        log=lambda m: print(m, file=sys.stderr))

    try:
        for (t, y, z, inten) in gen:
            if closed["v"]:                        # user closed the window
                break
            buf.append((y, z, inten))
            n_prof += 1
            n_pts += int(y.size)

            now = time.monotonic()
            if now - last_draw >= min_redraw:
                Y = np.concatenate([b[0] for b in buf])
                Z = np.concatenate([b[1] for b in buf])
                I = np.concatenate([b[2] for b in buf])
                sc.set_offsets(np.column_stack((Y, Z)))
                sc.set_array(I)
                m = max(np.abs(Y).max(), np.abs(Z).max(), 0.5) * 1.1
                ext = max(ext * 0.97, m)           # grow fast, settle slowly
                ax.set_xlim(-ext, ext)
                ax.set_ylim(-ext, ext)
                ax.set_title(f"Z+F live cross-section - {n_prof} profiles, "
                             f"{n_pts} pts, t={t:.1f}s")
                fig.canvas.draw_idle()
                last_draw = now
            fig.canvas.flush_events()
    except KeyboardInterrupt:
        pass
    finally:
        # the scan has stopped -> close the window
        if not closed["v"]:
            try:
                ax.set_title(f"scan stopped - {n_prof} profiles, {n_pts} pts (closing)")
                fig.canvas.draw_idle()
                plt.pause(0.6)
            except Exception:
                pass
            plt.close(fig)
        print(f"# done: {n_prof} profiles, {n_pts} points", file=sys.stderr)


if __name__ == "__main__":
    sys.exit(main())
