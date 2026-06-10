// plotter.cpp - real-time Y-Z cross-section plotter for the Z+F PROFILER 9020.
//
// C++ port of plot.py with interactive controls: opens an X11 window, draws
// each incoming profile's (Y, Z) points live (red cross = scanner origin) and
// provides Start/Stop buttons. The DEFAULT look matches Z+F's own viewer
// (scan1.png): light style (pink bg, black points) with the horizontal axis
// mirrored (+Y to the LEFT). --no-flip-y restores the manual frame
// (coordinate_system_definition.pdf, +Y right); --style dark gives the
// intensity-coloured view with a colourbar.
//
// Modes:
//   * --seconds given  -> timed: the scan auto-starts, runs N seconds, and the
//                         window closes when the scan stops (Stop ends early).
//   * --seconds omitted-> interactive: the window opens idle; the Start button
//                         starts the scanner, Stop stops it (repeatable). The
//                         window stays open until closed (or q/Esc).
//
// Each Start runs a fresh ZnF9020 session on a worker thread (profile callback
// feeds a shared buffer); the main thread owns the X11 event loop and redraws
// at --fps. Closing the window mid-scan stops the scan cleanly first.
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
#include <cstring>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

struct Profile {
    double t;
    std::vector<float> y, z, inten;
};

struct PlotArgs {
    int    persist    = 8;     // profiles kept on screen (fills stochastic dropouts)
    int    point_size = 3;     // marker size [px]
    double fps        = 20.0;  // max redraw rate
    bool   flip_y     = true;  // mirror the horizontal axis (Z+F viewer style)
    bool   light      = true;  // light style: pink bg, black points (scan1 look)
    bool   timed      = false; // true when --seconds was given on the CLI
};

static void usage() {
    fprintf(stderr,
        "usage: plotter <scanner-ip> [options]\n"
        "  --seconds N        timed mode: auto-start, run N seconds, auto-close.\n"
        "                     Omit for interactive mode with Start/Stop buttons.\n"
        "  --mode tcpzip|tcp  line stream encoding (default tcpzip)\n"
        "  --resolution R     scan resolution (default 10000)\n"
        "  --quality Q        scan quality (default 50)\n"
        "  --rps V            laser motor speed [rev/s]\n"
        "  --mhz V            laser data rate [MHz]\n"
        "  --min-amplitude A  drop returns below A (default 2000)\n"
        "  --rotate-deg D     angle-zero/mount correction (default 90.8, calibrated)\n"
        "  --min-range M      drop returns closer than M metres (default 0 = keep all)\n"
        "  --no-unwrap        disable gated >6.55 m unwrapping (cap at 6.55 m)\n"
        "  --mixpix-deg D     mixed-pixel edge-trail filter angle (default 8, 0=off)\n"
        "  --decimate N       use every Nth pixel (default 1)\n"
        "  --persist N        profiles kept on screen (default 8)\n"
        "  --point-size P     marker size in pixels (default 3)\n"
        "  --fps F            max redraw rate (default 20)\n"
        "  --no-flip-y        manual coordinate frame (+Y to the right);\n"
        "                     default mirrors the horizontal axis like Z+F's viewer\n"
        "  --style dark|light colour scheme (default light: pink background,\n"
        "                     black points, the scan1.png look; dark adds colourbar)\n"
        "  --local-ip IP      override local IP advertised to scanner\n");
}

static bool parse_args(int argc, char** argv, ZnF9020::Config& c, PlotArgs& p) {
    c.print_every = 1;                      // plotter default: every pixel
    for (int i = 1; i < argc; ++i) {
        std::string k = argv[i];
        auto val = [&](const char* name) -> const char* {
            if (i + 1 >= argc) { fprintf(stderr, "missing value for %s\n", name); exit(2); }
            return argv[++i];
        };
        if (k == "-h" || k == "--help") { usage(); exit(0); }
        else if (k == "--seconds")      { c.seconds = atof(val("--seconds")); p.timed = true; }
        else if (k == "--mode")           c.mode        = val("--mode");
        else if (k == "--resolution")     c.resolution  = val("--resolution");
        else if (k == "--quality")        c.quality     = val("--quality");
        else if (k == "--rps")          { c.rps = atof(val("--rps")); c.has_rps = true; }
        else if (k == "--mhz")          { c.mhz = atof(val("--mhz")); c.has_mhz = true; }
        else if (k == "--min-amplitude")  c.min_amp     = atof(val("--min-amplitude"));
        else if (k == "--rotate-deg")     c.rotate_deg  = atof(val("--rotate-deg"));
        else if (k == "--min-range")      c.min_range_m = atof(val("--min-range"));
        else if (k == "--no-unwrap")      c.unwrap = false;
        else if (k == "--mixpix-deg")     c.mixpix_deg = atof(val("--mixpix-deg"));
        else if (k == "--decimate")       c.print_every = atoi(val("--decimate"));
        else if (k == "--persist")        p.persist     = atoi(val("--persist"));
        else if (k == "--point-size")     p.point_size  = atoi(val("--point-size"));
        else if (k == "--fps")            p.fps         = atof(val("--fps"));
        else if (k == "--flip-y")         p.flip_y      = true;
        else if (k == "--no-flip-y")      p.flip_y      = false;
        else if (k == "--style")        { std::string v = val("--style");
                                          if (v != "light" && v != "dark") {
                                              fprintf(stderr, "unknown style %s (use dark|light)\n", v.c_str());
                                              return false;
                                          }
                                          p.light = (v == "light"); }
        else if (k == "--local-ip")       c.local_ip    = val("--local-ip");
        else if (!k.empty() && k[0] == '-') { fprintf(stderr, "unknown option %s\n", k.c_str()); return false; }
        else if (c.ip.empty())            c.ip = k;
        else { fprintf(stderr, "unexpected arg %s\n", k.c_str()); return false; }
    }
    if (p.persist < 1) p.persist = 1;
    if (p.point_size < 1) p.point_size = 1;
    if (!p.timed) c.seconds = 1e9;          // interactive: run until Stop
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

struct Btn {
    int x, y, w, h;
    const char* label;
    bool contains(int mx, int my) const {
        return mx >= x && my >= y && mx < x + w && my < y + h;
    }
};

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
    XSelectInput(dpy, win,
                 StructureNotifyMask | KeyPressMask | ExposureMask | ButtonPressMask);
    Atom wmDelete = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(dpy, win, &wmDelete, 1);
    XStoreName(dpy, win, pa.timed ? "AutoMap's Z+F live Y-Z - connecting ..."
                                  : "AutoMap's Z+F live Y-Z - press Start");
    XMapWindow(dpy, win);
    GC gc = XCreateGC(dpy, win, 0, nullptr);
    XFontStruct* font = XLoadQueryFont(dpy, "fixed");   // for labels
    if (font) XSetFont(dpy, gc, font->fid);
    int depth = DefaultDepth(dpy, scr);
    Pixmap pm = XCreatePixmap(dpy, win, W, H, depth);
    XFlush(dpy);

    // ---- shared profile buffer fed by the scanner worker thread ----------- //
    std::mutex mu;
    std::deque<Profile> buf;
    std::atomic<long> nprof{0}, npts{0};
    std::atomic<bool> session_done{false};
    std::atomic<bool> session_ok{false};

    auto onProfile = [&](double t, const std::vector<float>& y,
                         const std::vector<float>& z, const std::vector<float>& in) {
        std::lock_guard<std::mutex> lk(mu);
        buf.push_back({t, y, z, in});
        while ((int)buf.size() > pa.persist) buf.pop_front();
        ++nprof;
        npts += (long)y.size();
    };

    // ---- session management (one fresh ZnF9020 per Start) ----------------- //
    std::unique_ptr<ZnF9020> scanner;
    std::thread worker;
    bool running = false;
    long sessions = 0;
    int  rc = 0;
    double ext = 0.5;                       // half-extent of the square view [m]

    auto startSession = [&]() {
        if (running) return;
        {
            std::lock_guard<std::mutex> lk(mu);
            buf.clear();
        }
        nprof = 0; npts = 0; ext = 0.5;
        session_done = false;
        scanner = std::make_unique<ZnF9020>(cfg);
        scanner->setProfileCallback(onProfile);
        worker = std::thread([&] {
            session_ok = scanner->start();
            session_done = true;
        });
        running = true;
        ++sessions;
        fprintf(stderr, "# session %ld started\n", sessions);
    };
    auto stopSession = [&]() {              // graceful: worker reaps via session_done
        if (running && scanner) scanner->requestStop();
    };

    if (pa.timed) startSession();           // timed mode auto-starts

    // ---- UI loop ----------------------------------------------------------- //
    const Btn bStart{10, 10, 64, 26, "Start"};
    const Btn bStop {82, 10, 64, 26, "Stop"};
    const double frame_dt = 1.0 / (pa.fps > 1.0 ? pa.fps : 1.0);
    bool user_closed = false;
    double last_title = 0.0;
    auto secs = [] {
        using c = std::chrono::steady_clock;
        return std::chrono::duration<double>(c::now().time_since_epoch()).count();
    };

    bool finished = false;
    while (!finished) {
        // X events
        while (XPending(dpy)) {
            XEvent ev;
            XNextEvent(dpy, &ev);
            if (ev.type == ClientMessage && (Atom)ev.xclient.data.l[0] == wmDelete) {
                user_closed = true;
            } else if (ev.type == KeyPress) {
                KeySym ks = XLookupKeysym(&ev.xkey, 0);
                if (ks == XK_Escape || ks == XK_q) user_closed = true;
            } else if (ev.type == ButtonPress && ev.xbutton.button == Button1) {
                if (bStart.contains(ev.xbutton.x, ev.xbutton.y))      startSession();
                else if (bStop.contains(ev.xbutton.x, ev.xbutton.y)) stopSession();
            } else if (ev.type == ConfigureNotify &&
                       (ev.xconfigure.width != W || ev.xconfigure.height != H)) {
                W = ev.xconfigure.width;
                H = ev.xconfigure.height;
                XFreePixmap(dpy, pm);
                pm = XCreatePixmap(dpy, win, W, H, depth);
            }
        }
        if (user_closed) break;

        // reap a finished session (scan stopped on its own or via Stop)
        if (running && session_done) {
            worker.join();
            scanner.reset();
            running = false;
            rc = session_ok ? 0 : 1;
            if (pa.timed) { finished = true; break; }   // timed: close window
        }

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

        // draw into the back pixmap; a strip on the right is reserved for the
        // intensity colourbar (skipped if the window is too narrow)
        const int legendW = 84;
        const int Wp = (W > legendW + 160) ? W - legendW : W;   // plot-area width
        const int cx = Wp / 2, cy = H / 2;
        const int half = std::min(Wp, H) / 2 - 20;
        const double s = half / ext;        // pixels per metre
        XSetForeground(dpy, gc, pa.light ? 0xF8C8C8 : 0x141419);
        XFillRectangle(dpy, pm, gc, 0, 0, W, H);
        if (s >= 12.0) {                    // fine 1 m grid when it resolves
            XSetForeground(dpy, gc, pa.light ? 0xEDB6B6 : 0x2e2e38);
            for (int gm = 1; gm * s <= half; ++gm) {
                if (gm % 5 == 0) continue;  // 5 m lines drawn brighter below
                int d = (int)(gm * s);
                XDrawLine(dpy, pm, gc, cx - d, 0, cx - d, H);
                XDrawLine(dpy, pm, gc, cx + d, 0, cx + d, H);
                XDrawLine(dpy, pm, gc, 0, cy - d, Wp, cy - d);
                XDrawLine(dpy, pm, gc, 0, cy + d, Wp, cy + d);
            }
        }
        // 5 m x 5 m grid with distance labels (for estimating range)
        if (5.0 * s >= 8.0) {
            char lab[16];
            for (int gm = 5; gm * s <= half; gm += 5) {
                int d = (int)(gm * s);
                XSetForeground(dpy, gc, pa.light ? 0xE3A8A8 : 0x44445a);
                XDrawLine(dpy, pm, gc, cx - d, 0, cx - d, H);
                XDrawLine(dpy, pm, gc, cx + d, 0, cx + d, H);
                XDrawLine(dpy, pm, gc, 0, cy - d, Wp, cy - d);
                XDrawLine(dpy, pm, gc, 0, cy + d, Wp, cy + d);
                XSetForeground(dpy, gc, pa.light ? 0x5a5a5a : 0x9aa0b4);
                const int yd = pa.flip_y ? -d : d;   // +Y side on screen
                snprintf(lab, sizeof(lab), "%dm", gm);          // +Y / +Z
                XDrawString(dpy, pm, gc, cx + yd + 3, cy + 12, lab, (int)strlen(lab));
                XDrawString(dpy, pm, gc, cx + 4, cy - d - 4, lab, (int)strlen(lab));
                snprintf(lab, sizeof(lab), "-%dm", gm);         // -Y / -Z
                XDrawString(dpy, pm, gc, cx - yd + 3, cy + 12, lab, (int)strlen(lab));
                XDrawString(dpy, pm, gc, cx + 4, cy + d - 4, lab, (int)strlen(lab));
            }
        }
        XSetForeground(dpy, gc, pa.light ? 0xB9B9B9 : 0x484855);  // axes
        XDrawLine(dpy, pm, gc, cx, 0, cx, H);
        XDrawLine(dpy, pm, gc, 0, cy, Wp, cy);
        XSetForeground(dpy, gc, pa.light ? 0x404040 : 0xd0d4e6);  // axis names
        if (pa.flip_y) {                    // +Y points screen-left when mirrored
            XDrawString(dpy, pm, gc, 10, cy - 8, "+Y [m]", 6);
            XDrawLine(dpy, pm, gc, 0, cy, 8, cy - 4);         // Y arrowhead
            XDrawLine(dpy, pm, gc, 0, cy, 8, cy + 4);
        } else {
            XDrawString(dpy, pm, gc, Wp - 46, cy - 8, "+Y [m]", 6);
            XDrawLine(dpy, pm, gc, Wp - 1, cy, Wp - 9, cy - 4);
            XDrawLine(dpy, pm, gc, Wp - 1, cy, Wp - 9, cy + 4);
        }
        XDrawString(dpy, pm, gc, cx + 8, 16, "+Z [m]", 6);
        XDrawLine(dpy, pm, gc, cx, 0, cx - 4, 8);             // Z arrowhead
        XDrawLine(dpy, pm, gc, cx, 0, cx + 4, 8);
        const int ps = pa.point_size;
        {   // colour-batched: 32 buckets -> ~32 X requests instead of 2/point
            static std::vector<XRectangle> bucket[32];
            for (auto& b : bucket) b.clear();
            for (const auto& pr : snap)
                for (size_t i = 0; i < pr.y.size(); ++i) {
                    double yv = pa.flip_y ? -pr.y[i] : pr.y[i];
                    int sx = cx + (int)(yv * s);
                    int sy = cy - (int)(pr.z[i] * s);
                    if (sx >= 0 && sy >= 0 && sx < Wp && sy < H) {
                        int b = (int)(pr.inten[i] * (31.0f / 60000.0f));
                        if (b < 0) b = 0; else if (b > 31) b = 31;
                        bucket[b].push_back({(short)(sx - ps / 2),
                                             (short)(sy - ps / 2),
                                             (unsigned short)ps, (unsigned short)ps});
                    }
                }
            for (int b = 0; b < 32; ++b)
                if (!bucket[b].empty()) {
                    XSetForeground(dpy, gc, pa.light ? 0x000000
                                  : colorOf((b + 0.5f) * (60000.0f / 32.0f)));
                    XFillRectangles(dpy, pm, gc, bucket[b].data(), (int)bucket[b].size());
                }
        }
        XSetForeground(dpy, gc, 0xe04040);  // scanner origin
        XDrawLine(dpy, pm, gc, cx - 8, cy, cx + 8, cy);
        XDrawLine(dpy, pm, gc, cx, cy - 8, cx, cy + 8);

        if (Wp < W && !pa.light) {         // colourbar (skipped in light style)
            const int bx = W - legendW + 10;            // bar left edge
            const int bw = 16;
            const int bt = 46;
            const int bb = (H - 30 > bt + 40) ? H - 30 : bt + 40;
            const int bh = bb - bt;
            for (int r = 0; r <= bh; ++r) {             // gradient, top = max
                float frac = 1.0f - (float)r / (float)bh;
                XSetForeground(dpy, gc, colorOf(frac * 60000.0f));
                XDrawLine(dpy, pm, gc, bx, bt + r, bx + bw, bt + r);
            }
            XSetForeground(dpy, gc, 0x707080);          // frame
            XDrawRectangle(dpy, pm, gc, bx, bt, bw, bh);
            XSetForeground(dpy, gc, 0xc0c0cc);          // title + ticks
            XDrawString(dpy, pm, gc, bx - 6, bt - 16, "intensity", 9);
            static const struct { float frac; const char* lab; } ticks[] = {
                {1.00f, "60k"}, {0.75f, "45k"}, {0.50f, "30k"},
                {0.25f, "15k"}, {0.00f, "0"},
            };
            for (const auto& tk : ticks) {
                int ty = bt + (int)((1.0f - tk.frac) * bh);
                XDrawLine(dpy, pm, gc, bx + bw, ty, bx + bw + 4, ty);
                XDrawString(dpy, pm, gc, bx + bw + 7, ty + 4,
                            tk.lab, (int)strlen(tk.lab));
            }
        }

        // Start / Stop buttons (drawn last so they stay on top)
        auto drawBtn = [&](const Btn& b, bool enabled, unsigned long fill) {
            XSetForeground(dpy, gc, enabled ? fill : 0x24242c);
            XFillRectangle(dpy, pm, gc, b.x, b.y, b.w, b.h);
            XSetForeground(dpy, gc, 0x707080);
            XDrawRectangle(dpy, pm, gc, b.x, b.y, b.w, b.h);
            XSetForeground(dpy, gc, enabled ? 0xf0f0f0 : 0x606068);
            int len = (int)strlen(b.label);
            int tw = font ? XTextWidth(font, b.label, len) : 6 * len;
            XDrawString(dpy, pm, gc, b.x + (b.w - tw) / 2, b.y + b.h / 2 + 4,
                        b.label, len);
        };
        drawBtn(bStart, !running, 0x2e7d4f);
        drawBtn(bStop,  running,  0xa03434);

        XCopyArea(dpy, pm, win, gc, 0, 0, W, H, 0, 0);

        if (secs() - last_title > 0.5) {
            char title[160];
            if (running) {
                double tlast = snap.empty() ? 0.0 : snap.back().t;
                snprintf(title, sizeof(title),
                         "AutoMap's Z+F live Y-Z - %ld profiles, %ld pts - ext %.1f m - t=%.1f s",
                         nprof.load(), npts.load(), ext, tlast);
            } else if (sessions == 0) {
                snprintf(title, sizeof(title), "AutoMap's Z+F live Y-Z - press Start");
            } else {
                snprintf(title, sizeof(title),
                         "AutoMap's Z+F stopped - %ld profiles, %ld pts - press Start",
                         nprof.load(), npts.load());
            }
            XStoreName(dpy, win, title);
            last_title = secs();
        }
        XFlush(dpy);
        std::this_thread::sleep_for(std::chrono::duration<double>(frame_dt));
    }

    // wind down: stop a running scan, join the worker
    if (user_closed && running)
        fprintf(stderr, "# window closed - stopping scan\n");
    if (scanner) scanner->requestStop();
    if (worker.joinable()) worker.join();
    if (scanner) { rc = session_ok ? 0 : 1; scanner.reset(); }

    if (pa.timed && !user_closed) {         // timed: brief "closing" notice
        char title[128];
        snprintf(title, sizeof(title),
                 "scan stopped - %ld profiles, %ld pts (closing)",
                 nprof.load(), npts.load());
        XStoreName(dpy, win, title);
        XFlush(dpy);
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
    }
    XFreePixmap(dpy, pm);
    if (font) XFreeFont(dpy, font);
    XFreeGC(dpy, gc);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
    fprintf(stderr, "# done: %ld session(s), last: %ld profiles, %ld points\n",
            sessions, nprof.load(), npts.load());
    return rc;
}
