// plotter.cpp - real-time Y-Z cross-section plotter for the Z+F PROFILER 9020.
//
// C++ port of plot.py: opens an X11 window, draws each incoming profile's
// (Y, Z) points live (colour = intensity, red cross = scanner origin), and
// closes automatically when the scan stops (--seconds elapsed or stream drop).
// Closing the window (or pressing q/Esc) stops the scan.
//
// The scanner runs on a worker thread via ZnF9020 with a profile callback;
// the main thread owns the X11 event loop and redraws at --fps.
#include "ZnF9020.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <atomic>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

struct Profile {
    double t;
    std::vector<float> y, z, inten;
};

struct PlotArgs {
    int    persist    = 1;     // profiles kept on screen
    int    point_size = 3;     // marker size [px]
    double fps        = 20.0;  // max redraw rate
};

static void usage() {
    fprintf(stderr,
        "usage: plotter <scanner-ip> [options]\n"
        "  --seconds N        acquire duration (default 30)\n"
        "  --mode tcpzip|tcp  line stream encoding (default tcpzip)\n"
        "  --resolution R     scan resolution (default 10000)\n"
        "  --quality Q        scan quality (default 50)\n"
        "  --rps V            laser motor speed [rev/s]\n"
        "  --mhz V            laser data rate [MHz]\n"
        "  --min-amplitude A  drop returns below A (default 2000)\n"
        "  --decimate N       use every Nth pixel (default 2)\n"
        "  --persist N        profiles kept on screen (default 1)\n"
        "  --point-size P     marker size in pixels (default 3)\n"
        "  --fps F            max redraw rate (default 20)\n"
        "  --local-ip IP      override local IP advertised to scanner\n");
}

static bool parse_args(int argc, char** argv, ZnF9020::Config& c, PlotArgs& p) {
    c.print_every = 2;                      // plotter default: every 2nd pixel
    for (int i = 1; i < argc; ++i) {
        std::string k = argv[i];
        auto val = [&](const char* name) -> const char* {
            if (i + 1 >= argc) { fprintf(stderr, "missing value for %s\n", name); exit(2); }
            return argv[++i];
        };
        if (k == "-h" || k == "--help") { usage(); exit(0); }
        else if (k == "--seconds")        c.seconds     = atof(val("--seconds"));
        else if (k == "--mode")           c.mode        = val("--mode");
        else if (k == "--resolution")     c.resolution  = val("--resolution");
        else if (k == "--quality")        c.quality     = val("--quality");
        else if (k == "--rps")          { c.rps = atof(val("--rps")); c.has_rps = true; }
        else if (k == "--mhz")          { c.mhz = atof(val("--mhz")); c.has_mhz = true; }
        else if (k == "--min-amplitude")  c.min_amp     = atof(val("--min-amplitude"));
        else if (k == "--decimate")       c.print_every = atoi(val("--decimate"));
        else if (k == "--persist")        p.persist     = atoi(val("--persist"));
        else if (k == "--point-size")     p.point_size  = atoi(val("--point-size"));
        else if (k == "--fps")            p.fps         = atof(val("--fps"));
        else if (k == "--local-ip")       c.local_ip    = val("--local-ip");
        else if (!k.empty() && k[0] == '-') { fprintf(stderr, "unknown option %s\n", k.c_str()); return false; }
        else if (c.ip.empty())            c.ip = k;
        else { fprintf(stderr, "unexpected arg %s\n", k.c_str()); return false; }
    }
    if (p.persist < 1) p.persist = 1;
    if (p.point_size < 1) p.point_size = 1;
    return !c.ip.empty();
}

// viridis-like intensity colour: 0 -> dark purple, 60000 -> yellow
static unsigned long colorOf(float inten) {
    float t = inten / 60000.0f;
    if (t < 0.f) t = 0.f;
    if (t > 1.f) t = 1.f;
    float r, g, b;
    if (t < 0.5f) {
        float u = t * 2.f;
        r = 68.f + u * (33.f - 68.f);
        g = 1.f  + u * (145.f - 1.f);
        b = 84.f + u * (140.f - 84.f);
    } else {
        float u = (t - 0.5f) * 2.f;
        r = 33.f  + u * (253.f - 33.f);
        g = 145.f + u * (231.f - 145.f);
        b = 140.f + u * (37.f - 140.f);
    }
    return ((unsigned long)r << 16) | ((unsigned long)g << 8) | (unsigned long)b;
}

int main(int argc, char** argv) {
    signal(SIGPIPE, SIG_IGN);
    ZnF9020::Config cfg;
    PlotArgs pa;
    if (!parse_args(argc, argv, cfg, pa)) { usage(); return 2; }

    Display* dpy = XOpenDisplay(nullptr);
    if (!dpy) {
        fprintf(stderr, "Cannot open X display - run on a desktop session "
                        "(DISPLAY set), or use ./acquire for text output.\n");
        return 1;
    }
    int scr = DefaultScreen(dpy);
    int W = 840, H = 840;
    Window win = XCreateSimpleWindow(dpy, RootWindow(dpy, scr), 0, 0, W, H, 0,
                                     BlackPixel(dpy, scr), BlackPixel(dpy, scr));
    XSelectInput(dpy, win, StructureNotifyMask | KeyPressMask | ExposureMask);
    Atom wmDelete = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(dpy, win, &wmDelete, 1);
    XStoreName(dpy, win, "Z+F live Y-Z - connecting ...");
    XMapWindow(dpy, win);
    GC gc = XCreateGC(dpy, win, 0, nullptr);
    int depth = DefaultDepth(dpy, scr);
    Pixmap pm = XCreatePixmap(dpy, win, W, H, depth);
    XFlush(dpy);

    // shared profile buffer fed by the scanner worker thread
    std::mutex mu;
    std::deque<Profile> buf;
    std::atomic<long> nprof{0}, npts{0};
    std::atomic<bool> done{false};
    std::atomic<bool> ok{false};

    ZnF9020 scanner(cfg);
    scanner.setProfileCallback(
        [&](double t, const std::vector<float>& y, const std::vector<float>& z,
            const std::vector<float>& inten) {
            std::lock_guard<std::mutex> lk(mu);
            buf.push_back({t, y, z, inten});
            while ((int)buf.size() > pa.persist) buf.pop_front();
            ++nprof;
            npts += (long)y.size();
        });
    std::thread worker([&] { ok = scanner.start(); done = true; });

    const double frame_dt = 1.0 / (pa.fps > 1.0 ? pa.fps : 1.0);
    double ext = 0.5;                       // half-extent of the square view [m]
    bool user_closed = false;
    double last_title = 0.0;
    auto secs = [] {
        using c = std::chrono::steady_clock;
        return std::chrono::duration<double>(c::now().time_since_epoch()).count();
    };

    while (!done) {
        // X events
        while (XPending(dpy)) {
            XEvent ev;
            XNextEvent(dpy, &ev);
            if (ev.type == ClientMessage && (Atom)ev.xclient.data.l[0] == wmDelete) {
                user_closed = true;
            } else if (ev.type == KeyPress) {
                KeySym ks = XLookupKeysym(&ev.xkey, 0);
                if (ks == XK_Escape || ks == XK_q) user_closed = true;
            } else if (ev.type == ConfigureNotify &&
                       (ev.xconfigure.width != W || ev.xconfigure.height != H)) {
                W = ev.xconfigure.width;
                H = ev.xconfigure.height;
                XFreePixmap(dpy, pm);
                pm = XCreatePixmap(dpy, win, W, H, depth);
            }
        }
        if (user_closed) break;

        // snapshot the shared buffer
        std::vector<Profile> snap;
        {
            std::lock_guard<std::mutex> lk(mu);
            snap.assign(buf.begin(), buf.end());
        }

        // autoscale: grow fast, settle slowly; square aspect
        float m = 0.5f;
        for (const auto& pr : snap)
            for (size_t i = 0; i < pr.y.size(); ++i) {
                float ay = std::fabs(pr.y[i]), az = std::fabs(pr.z[i]);
                if (ay > m) m = ay;
                if (az > m) m = az;
            }
        ext = std::max(ext * 0.97, 1.1 * (double)m);
        if (ext < 0.5) ext = 0.5;

        // draw into the back pixmap
        const int cx = W / 2, cy = H / 2;
        const int half = std::min(W, H) / 2 - 20;
        const double s = half / ext;        // pixels per metre
        XSetForeground(dpy, gc, 0x141419);
        XFillRectangle(dpy, pm, gc, 0, 0, W, H);
        if (s >= 12.0) {                    // 1 m grid when it resolves
            XSetForeground(dpy, gc, 0x2e2e38);
            for (int gm = 1; gm * s <= half; ++gm) {
                int d = (int)(gm * s);
                XDrawLine(dpy, pm, gc, cx - d, 0, cx - d, H);
                XDrawLine(dpy, pm, gc, cx + d, 0, cx + d, H);
                XDrawLine(dpy, pm, gc, 0, cy - d, W, cy - d);
                XDrawLine(dpy, pm, gc, 0, cy + d, W, cy + d);
            }
        }
        XSetForeground(dpy, gc, 0x484855);  // axes
        XDrawLine(dpy, pm, gc, cx, 0, cx, H);
        XDrawLine(dpy, pm, gc, 0, cy, W, cy);
        const int ps = pa.point_size;
        for (const auto& pr : snap)
            for (size_t i = 0; i < pr.y.size(); ++i) {
                int sx = cx + (int)(pr.y[i] * s);
                int sy = cy - (int)(pr.z[i] * s);
                if (sx >= 0 && sy >= 0 && sx < W && sy < H) {
                    XSetForeground(dpy, gc, colorOf(pr.inten[i]));
                    XFillRectangle(dpy, pm, gc, sx - ps / 2, sy - ps / 2, ps, ps);
                }
            }
        XSetForeground(dpy, gc, 0xe04040);  // scanner origin
        XDrawLine(dpy, pm, gc, cx - 8, cy, cx + 8, cy);
        XDrawLine(dpy, pm, gc, cx, cy - 8, cx, cy + 8);
        XCopyArea(dpy, pm, win, gc, 0, 0, W, H, 0, 0);

        if (secs() - last_title > 0.5) {
            char title[160];
            double tlast = snap.empty() ? 0.0 : snap.back().t;
            snprintf(title, sizeof(title),
                     "Z+F live Y-Z - %ld profiles, %ld pts - ext %.1f m - t=%.1f s",
                     nprof.load(), npts.load(), ext, tlast);
            XStoreName(dpy, win, title);
            last_title = secs();
        }
        XFlush(dpy);
        std::this_thread::sleep_for(std::chrono::duration<double>(frame_dt));
    }

    if (user_closed)
        fprintf(stderr, "# window closed - stopping scan\n");
    scanner.requestStop();
    worker.join();

    if (!user_closed) {                     // scan stopped -> close the window
        char title[128];
        snprintf(title, sizeof(title),
                 "scan stopped - %ld profiles, %ld pts (closing)",
                 nprof.load(), npts.load());
        XStoreName(dpy, win, title);
        XFlush(dpy);
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
    }
    XFreePixmap(dpy, pm);
    XFreeGC(dpy, gc);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
    fprintf(stderr, "# done: %ld profiles, %ld points\n", nprof.load(), npts.load());
    return ok ? 0 : 1;
}
