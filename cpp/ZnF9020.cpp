// ZnF9020.cpp - implementation of the Z+F PROFILER 9020 controller.
#include "ZnF9020.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cerrno>
#include <ctime>
#include <chrono>
#include <thread>

#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <zlib.h>

static double nowSeconds() {
    using clk = std::chrono::steady_clock;
    return std::chrono::duration<double>(clk::now().time_since_epoch()).count();
}
static void sleepSeconds(double s) {
    std::this_thread::sleep_for(std::chrono::duration<double>(s));
}

// poll() timeouts: while idle between frames we wake every MARKER_TIMEOUT_S so
// StreamFor() can check its deadline; a stall in the MIDDLE of a frame or while
// sending a command is treated as an error/failure instead of waiting forever.
static constexpr double MARKER_TIMEOUT_S     = 1.0;
static constexpr double FRAME_IDLE_TIMEOUT_S = 3.0;
static constexpr double SEND_TIMEOUT_S       = 5.0;

// --------------------------------------------------------------------------- //
// UniqueFd - RAII socket descriptor (close() lives here and nowhere else)
// --------------------------------------------------------------------------- //
ZnF9020::UniqueFd::~UniqueFd() { Reset(); }

ZnF9020::UniqueFd& ZnF9020::UniqueFd::operator=(UniqueFd&& o) noexcept {
    if (this != &o) { Reset(o.m_fd); o.m_fd = -1; }
    return *this;
}

void ZnF9020::UniqueFd::Reset(int fd) noexcept {
    if (m_fd >= 0) close(m_fd);
    m_fd = fd;
}

// --------------------------------------------------------------------------- //
// lifecycle
// --------------------------------------------------------------------------- //
ZnF9020::ZnF9020(const Config& cfg) : m_cfg(cfg) {}
ZnF9020::~ZnF9020() { Stop(); Disconnect(); }

bool ZnF9020::Start() {
    m_local = m_cfg.local_ip.empty() ? localIp(m_cfg.ip) : m_cfg.local_ip;
    m_scanCmd = buildScanCommand();              // built once (unique file name)
    fprintf(stderr, "# connecting to scanner %s (command :%d, stream :%d)\n",
            m_cfg.ip.c_str(), PORT_COMMAND, PORT_STREAM);
    fprintf(stderr, "# local ip advertised: %s\n", m_local.c_str());
    fprintf(stderr, "# scan command: %s\n", m_scanCmd.c_str());

    if (!Connect()) {
        fprintf(stderr, "!! cannot reach scanner command port %s:%d\n",
                m_cfg.ip.c_str(), PORT_COMMAND);
        if (m_local == m_cfg.ip)
            fprintf(stderr, "   (that is THIS PC's own IP - use the scanner's address)\n");
        return false;
    }
    if (!StartScan()) { Stop(); return false; }

    if (!m_onProfile)
        printf("# t[s]      x[m]      y[m]      z[m]   intensity\n");
    StreamFor(m_cfg.seconds);
    Stop();
    return true;
}

// --------------------------------------------------------------------------- //
// command channel
// --------------------------------------------------------------------------- //
bool ZnF9020::Connect() {
    m_cmdFd.Reset(connectTcp(m_cfg.ip, PORT_COMMAND));
    return m_cmdFd.Valid();
}

std::string ZnF9020::Command(const std::string& text, double wait) {
    if (!m_cmdFd.Valid()) return "";
    if (!sendAll(m_cmdFd.Get(), text + "\r\n", SEND_TIMEOUT_S)) return "";
    std::string reply;
    char buf[4096];
    for (;;) {
        if (!waitFd(m_cmdFd.Get(), POLLIN, wait)) break; // no (more) data in time
        const ssize_t r = recv(m_cmdFd.Get(), buf, sizeof(buf), 0);
        if (r > 0) {
            reply.append(buf, (size_t)r);
            if (reply.find("</xml>") != std::string::npos) break;
        } else if (r == 0) {
            break;                                       // peer closed
        } else if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
            continue;
        } else break;
    }
    return reply;
}

void ZnF9020::EnsureIdle(double wait) {
    const long BUSY = (1<<1)|(1<<2)|(1<<3)|(1<<15);   // STREAM|SCAN|ERR|STARTUP
    std::string st;
    for (int i = 0; i < 12; ++i) {
        st = Command("status", wait);
        if (st.find("<messagebox") != std::string::npos) {
            Command(affirmative(st), wait); sleepSeconds(0.3); continue;
        }
        long s = 0;
        if (statusInt(st, s) && (s & BUSY)) { Command("stop", wait); sleepSeconds(0.5); continue; }
        break;
    }
    long s = 0;
    fprintf(stderr, "# status (idle): %s\n",
            statusInt(st, s) ? statusStr(s).c_str() : st.c_str());
}

// --------------------------------------------------------------------------- //
// scanning / streaming
// --------------------------------------------------------------------------- //
bool ZnF9020::StartScan() {
    m_dupLog = 0; m_burstCool = 0;           // fresh per-scan diagnostics
    fprintf(stderr, "# version: %.80s ...\n", Command("version").c_str());
    EnsureIdle();
    sleepSeconds(0.5);
    fprintf(stderr, "# scan reply: %s\n", Command(m_scanCmd).c_str());
    m_scanning = true;
    for (int i = 0; i < 10 && !m_abort.load(); ++i) { // auto-answer overwrite/messagebox
        sleepSeconds(0.5);
        std::string st = Command("status");
        if (st.find("<messagebox") != std::string::npos) {
            fprintf(stderr, "# messagebox -> answering '%s'\n", affirmative(st).c_str());
            Command(affirmative(st));
            continue;
        }
        long s = 0;
        fprintf(stderr, "# status (after): %s\n",
                statusInt(st, s) ? statusStr(s).c_str() : st.c_str());
        break;
    }
    double deadline = nowSeconds() + 10;
    while (!m_streamFd.Valid() && nowSeconds() < deadline && !m_abort.load()) {
        m_streamFd.Reset(connectTcp(m_cfg.ip, PORT_STREAM, 2.0));
        if (!m_streamFd.Valid()) sleepSeconds(0.5);
    }
    if (!m_streamFd.Valid()) { fprintf(stderr, "!! could not connect to stream port\n"); return false; }
    const int rcvbuf = 4 * 1024 * 1024;     // absorb GUI/X11 stalls without
    setsockopt(m_streamFd.Get(), SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));
    return true;
}

void ZnF9020::StreamFor(double seconds) {
    if (!m_streamFd.Valid()) return;
    double t_s = 0.0; long line_no = -1;
    bool got_data = false;
    double deadline = nowSeconds() + seconds + 30.0;  // provisional: allow ~12 s spin-up
    double last_stat = nowSeconds();
    fprintf(stderr, "# waiting for scanner spin-up (data-invalid phase) ...\n");
    int err_streak = 0;                          // consecutive recoverable faults
    while (nowSeconds() < deadline && !m_abort.load()) {
        uint16_t type;
        FrameRes fr = readFrame(type, m_payload);
        if (fr == FRAME_TIMEOUT) continue;
        if (fr == FRAME_ERR) {
            // single faults (device hiccup / mid-frame stall / desync) are
            // recoverable via the marker rescan; only a persistent failure
            // (e.g. the peer closed: instant errors back-to-back) is fatal
            if (++err_streak >= 10) { fprintf(stderr, "!! stream error (persistent)\n"); break; }
            continue;
        }
        err_streak = 0;

        if (type == ZFS_HEADER_ID) {
            int p = parseHeaderPixel(m_payload.data(), m_payload.size());
            if (p >= 1024 && p <= 32768 && p % 2 == 0) {
                if (m_pixel > 0 && p != m_pixel)
                    fprintf(stderr, "# pixel/line changed %d -> %d\n", m_pixel, p);
                m_pixel = p;
                fprintf(stderr, "# zfs header: pixel/line = %d\n", m_pixel);
            } else if (p > 0) {
                fprintf(stderr, "# ignoring implausible pixel/line %d from header frame\n", p);
            }
        } else if (type == LINE_HEADER_ID) {
            m_lineHeader = m_payload;
            t_s     = (m_lineHeader.size() >= 32) ? rdU32(m_lineHeader.data() + 28) / 1000.0 : 0.0;
            line_no = (m_lineHeader.size() >= 36) ? (long)rdU32(m_lineHeader.data() + 32) : -1;
        } else if (type == RAW_PIXEL_ID || type == COMP_PIXEL_ID) {
            const unsigned char* data = m_payload.data();
            size_t n = m_payload.size();
            if (type == COMP_PIXEL_ID) {
                n = inflateLine(m_payload.data(), m_payload.size(), m_inflated);
                data = m_inflated.data();
                if (n == 0) continue;
            }
            if (m_pixel < 0) {                    // fallback from payload size,
                int p = (int)(n / 8);            // same plausibility bounds
                if (p >= 1024 && p <= 32768 && p % 2 == 0) m_pixel = p;
                else continue;
            }
            long li = (line_no >= 0) ? line_no : m_lines;
            long got = decodeProfile(data, n, t_s, (double)li * m_cfg.spacing);
            m_pts += got;
            ++m_lines;
            if (!got_data && got > 0) {
                got_data = true;
                deadline = nowSeconds() + seconds;    // --seconds counts from first data
                fprintf(stderr, "# first valid data - %.0f s acquisition window starts\n", seconds);
            }
        }
        if (nowSeconds() - last_stat >= 1.0) {
            fprintf(stderr, "# ... %ld lines, %ld points\n", m_lines, m_pts);
            last_stat = nowSeconds();
        }
    }
    fprintf(stderr, "# acquired ~%ld lines, %ld points\n", m_lines, m_pts);
}

void ZnF9020::Stop() {
    if (m_scanning && m_cmdFd.Valid())
        fprintf(stderr, "# stop reply: %s\n", Command("stop").c_str());
    m_scanning = false;
}

void ZnF9020::Disconnect() {
    m_streamFd.Reset();
    m_cmdFd.Reset();
}

// --------------------------------------------------------------------------- //
// scan-command building, framing, decode
// --------------------------------------------------------------------------- //
std::string ZnF9020::buildScanCommand() const {
    if (!m_cfg.scan_args.empty()) return "scan " + m_cfg.scan_args;
    std::string fname = m_cfg.file;
    if (fname.empty()) {
        char ts[32]; time_t t = time(nullptr); tm lt; localtime_r(&t, &lt);
        strftime(ts, sizeof(ts), "live_%Y%m%d-%H%M%S", &lt);
        fname = ts;
    }
    std::string cmd = "scan -profiler -resolution " + m_cfg.resolution +
                      " -quality " + m_cfg.quality + " -file " + fname +
                      " -path scans -filesize 250mb -linestream " + m_cfg.mode +
                      " -statusstream -memorize 3 -writing 0 -ip " + m_local;
    char num[64];
    if (m_cfg.has_rps) { snprintf(num, sizeof(num), " %s %g", RPS_FLAG, m_cfg.rps); cmd += num; }
    if (m_cfg.has_mhz) { snprintf(num, sizeof(num), " %s %g", MHZ_FLAG, m_cfg.mhz); cmd += num; }
    if (!m_cfg.extra.empty()) cmd += " " + m_cfg.extra;
    return cmd;
}

ZnF9020::FrameRes ZnF9020::readFrame(uint16_t& type, std::vector<unsigned char>& payload) {
    // Hardened against device-side stream faults (a wrapped -memorize ring
    // resumes mid-frame; a false in-payload "Z+F\0" then yields a garbage
    // size field). A naive resize/recv of an unvalidated 32-bit size either
    // crashes (GB allocation) or silently swallows SECONDS of stream as one
    // bogus payload - seen live as a spontaneous multi-second outage.
    static constexpr uint32_t MAX_FRAME_BYTES = 4u << 20; // legit < ~1 MiB
    static constexpr long     MAX_SCAN_BYTES  = 4L << 20; // bound marker hunt
    unsigned char m[4];
    if (!recvExact(m_streamFd.Get(),m, 4, MARKER_TIMEOUT_S)) return FRAME_TIMEOUT;
    long scanned = 0;
    while (!(m[0]=='Z' && m[1]=='+' && m[2]=='F' && m[3]==0)) {      // resync to marker
        m[0]=m[1]; m[1]=m[2]; m[2]=m[3];
        if (!recvExact(m_streamFd.Get(),&m[3], 1, MARKER_TIMEOUT_S)) return FRAME_TIMEOUT;
        if (++scanned > MAX_SCAN_BYTES) return FRAME_ERR; // let caller breathe
    }
    if (scanned)
        fprintf(stderr, "# stream resync: %ld bytes skipped to next frame marker\n", scanned);
    unsigned char h[10];
    if (!recvExact(m_streamFd.Get(),h, 10, FRAME_IDLE_TIMEOUT_S)) return FRAME_ERR;
    type = (uint16_t)(h[0] | (h[1] << 8));
    uint32_t size = (uint32_t)h[2] | ((uint32_t)h[3]<<8) | ((uint32_t)h[4]<<16) | ((uint32_t)h[5]<<24);
    if (size > MAX_FRAME_BYTES) {
        fprintf(stderr, "# stream desync: implausible frame size %u (type %u) - resyncing\n",
                size, type);
        return FRAME_ERR;                            // recoverable: rescan next call
    }
    payload.resize(size);
    if (size && !recvExact(m_streamFd.Get(),payload.data(), size, FRAME_IDLE_TIMEOUT_S)) return FRAME_ERR;
    return FRAME_OK;
}

long ZnF9020::decodeProfile(const unsigned char* data, size_t n, double t_s, double x) {
    const uint16_t* u = (const uint16_t*)data;       // host is LE
    if (m_pixel <= 0 || n < (size_t)m_pixel * 8) return 0;
    // ABSOLUTE PER-PIXEL DECODE (discovered 2026-06-12). Live tcpzip layout,
    // all values byte-doubled coarse (high byte duplicated into low):
    //   plane0[half+j]  range modulo 6.5536 m      (x 0.1 mm, ~25.7 mm steps)
    //   plane1[j]       WRAP COUNT k               (value/257; 0..27 spans the
    //                                               full 182.68 m spec range)
    //   plane1[half+j]  amplitude
    //   plane2[j]       secondary range estimate   (x 0.2 mm, modulo 13.107 m;
    //                                               "parks" on codes when weak)
    //   true range r = plane0 + k * 6.5536  - absolute, no ambiguity, no
    //   cross-line state. plane2 serves only as a mixed-echo sanity check
    //   (compared MODULO its own interval). No-return STATUS pixels carry a
    //   duplicated-byte code in plane0 AND plane2 == the same raw value.
    static constexpr double AMBIG_M    = 6.5536;
    static constexpr double SENTINEL_M = 0.345;   // window/no-return zone
    static constexpr double SEC_MOD_M  = 13.1072; // plane2 own interval
    static constexpr double SEC_MIN_M  = 0.40;    // below this = code
    static constexpr double SEC_VETO_M = 2.5;     // mixed-echo disagreement
    static constexpr double R_MAX_M    = 182.68;  // FULL spec range (tag 50)
    // NEAR-FIELD OVERLOAD gates (characterized from a raw capture 2026-06-12;
    // see acquire.py decode_dual and test_decode.py). A hand <30 cm upsets a
    // sector of directions: during the event plane1 carries byte-doubled
    // garbage codes that decode as valid k (constant-radius ARCS at 38/94/
    // 144/177 m, interleaved with invalid-k pixels); afterwards the
    // directions stay PARKED for the rest of the scan at ~181.3-182 m with a
    // FROZEN amplitude code (e.g. 0xB4B4 +/- low-byte soup) - the
    // "saturation scar". Genuine amplitude jitters and genuine lines carry
    // ~zero invalid-k pixels, so two gates remove both artifact classes
    // (measured 99.6% catch, 0.0015% genuine loss) WITHOUT a range band cut:
    //   AMP-PARK  amplitude exactly repeats >= AMP_PARK_MIN among +-4 px
    //             (>= AMP_PARK_DUP_MIN if the value is byte-doubled);
    //   UPSET     a wrap claim (k>=1) within +-UPSET_NB px of corruption
    //             evidence (invalid-k pixel or amp-park) is sector garbage.
    static constexpr int    PARKED_NB  = 4;       // +/- neighbours checked
    static constexpr int    PARKED_MIN = 3;       // exact repeats => parked
    static constexpr int    AMP_PARK_MIN     = 3; // amp exact repeats in +-4
    static constexpr int    AMP_PARK_DUP_MIN = 2; // if amp value byte-doubled
    static constexpr int    UPSET_NB   = 8;       // evidence influence radius
    const int dirs = m_pixel / 2;
    const uint16_t* pf = u + dirs;                // plane0: range mod 6.5536
    const uint16_t* pk = u + m_pixel;              // plane1 first half: k
    const uint16_t* pa = u + m_pixel + dirs;       // plane1 second half: amp
    const uint16_t* pc = u + 2 * m_pixel;          // plane2: secondary
    const double rot = m_cfg.rotate_deg * M_PI / 180.0;
    auto dup = [](uint16_t v) { return (v & 0xFF) == (v >> 8); };
    std::vector<unsigned char> kbadpx(dirs, 0);      // invalid-k encoding flags

    {
        // DEVICE RECALIBRATION BURST (~41 s after scan start, ~60 lines):
        // plane0 collapses to one constant no-return code with strong AMBIENT
        // amplitude; the mixed entry/exit lines slip per-pixel gates. No real
        // scene puts >15% of a revolution into ONE coarse bin (healthy max
        // 7.3%; burst lines 32-100%). Drop them whole.
        static std::vector<uint16_t> cnt;
        cnt.assign(65536, 0);
        int modal = 0;
        for (int j = 0; j < dirs; ++j) {
            int v = ++cnt[pf[j]];
            if (v > modal) modal = v;
        }
        if (100 * modal > 15 * dirs) {
            if ((m_dupLog++ % 50) == 0)
                fprintf(stderr, "# device no-return burst dropped "
                        "(modal fine %d%% of line)\n", 100 * modal / dirs);
            m_burstCool = 3;                 // also skip the garbled exit lines
            return 0;
        }
        // WRAP-COUNTER INTEGRITY: plane1[j] is byte-doubled in every healthy
        // line ever captured (0 deviations >1). A broken k channel (e.g. a
        // device state upset after near-field saturation) would scatter
        // points to wild ranges <=183 m - drop such lines whole.
        int kbad = 0;
        for (int j = 0; j < dirs; ++j) {
            long kk = lround(pk[j] / 257.0);
            if (labs((long)pk[j] - kk * 257) > 1) { ++kbad; kbadpx[j] = 1; }
        }
        if (20 * kbad > dirs) {              // >5% broken = channel upset
            if ((m_dupLog++ % 50) == 0)
                fprintf(stderr, "# wrap-counter channel upset dropped "
                        "(%d/%d px invalid)\n", kbad, dirs);
            m_burstCool = 3;
            return 0;
        }
        if (m_burstCool > 0) { --m_burstCool; return 0; }
    }

    // near-field-overload gates: frozen-amplitude park + upset neighbourhood
    std::vector<unsigned char> amppark(dirs, 0), nearUpset(dirs, 0);
    for (int j = 0; j < dirs; ++j) {
        int rep = 0;
        for (int o = -PARKED_NB; o <= PARKED_NB; ++o) {
            if (!o) continue;
            if (pa[(j + o + dirs) % dirs] == pa[j]) ++rep;
        }
        const bool dupAmp = (pa[j] & 0xFF) == (pa[j] >> 8);
        amppark[j] = (rep >= AMP_PARK_MIN ||
                      (dupAmp && rep >= AMP_PARK_DUP_MIN)) ? 1 : 0;
    }
    for (int j = 0; j < dirs; ++j) {
        if (!(kbadpx[j] || amppark[j])) continue;
        for (int o = -UPSET_NB; o <= UPSET_NB; ++o)
            nearUpset[(j + o + dirs) % dirs] = 1;
    }

    // single pass: absolute range per pixel + validity
    std::vector<double> rbuf(dirs, 0.0), abuf(dirs, 0.0);
    std::vector<unsigned char> ok(dirs, 0);
    for (int j = 0; j < dirs; ++j) {
        abuf[j] = pa[j];
        if (dup(pf[j]) && pc[j] == pf[j]) continue;  // no-return status pixel
        if (abuf[j] <= m_cfg.min_amp) continue;
        if (amppark[j]) continue;                    // frozen-amp park / scar
        double fine = pf[j] * RANGE_SCALE_M;
        long   k    = lround(pk[j] / 257.0);         // byte-doubled wrap count
        if (labs((long)pk[j] - k * 257) > 1) continue; // invalid k encoding
        if (!m_cfg.unwrap) k = 0;                     // fold display (debug)
        if (k >= 1 && nearUpset[j]) continue;        // wrap claim in upset sector
        if (k == 0 && fine < SENTINEL_M) continue;   // window sentinel
        double r = fine + (double)k * AMBIG_M;
        if (r > R_MAX_M) continue;                   // beyond spec = junk
        if (r < m_cfg.min_range_m) continue;
        if (m_cfg.unwrap) {
            // plane2 sanity: when it MEASURES (parked codes exactly repeat
            // across neighbours; real values jitter), it must agree with r
            // modulo its own interval, else the pixel is a mixed echo
            double sec = pc[j] * 2.0 * RANGE_SCALE_M;
            if (sec >= SEC_MIN_M) {
                int parked = 0;
                for (int o = -PARKED_NB; o <= PARKED_NB; ++o) {
                    if (!o) continue;
                    if (pc[(j + o + dirs) % dirs] == pc[j]) ++parked;
                }
                if (parked < PARKED_MIN) {
                    double m = std::fmod(r, SEC_MOD_M);
                    double d = std::fabs(sec - m);
                    d = std::min(d, SEC_MOD_M - d);
                    if (d > SEC_VETO_M) continue;    // plane2 saw another echo
                }
            }
        }
        rbuf[j] = r; ok[j] = 1;
    }

    // pass 2: mixed-pixel (edge-trail) filter - drop points whose local chord
    // is nearly parallel to the beam, EXCEPT long runs of such points (a real
    // surface seen at grazing incidence, e.g. the far floor, is also chord-
    // radial but spans far more pixels than a spot-size-bounded edge trail).
    std::vector<unsigned char> drop(dirs, 0);
    if (m_cfg.mixpix_deg > 0.0) {
        static constexpr int MIX_RUN_EXEMPT = 32;    // px span = real surface
        static constexpr int MIX_RUN_GAP    = 8;     // px joining run members
        const int W = 4;
        const double cosT = std::cos(m_cfg.mixpix_deg * M_PI / 180.0);
        std::vector<unsigned char> flag(dirs, 0);
        for (int j = W; j < dirs - W; ++j) {
            if (!ok[j] || !ok[j - W] || !ok[j + W]) continue;
            double a0 = 2.0 * M_PI * (j - W) / dirs + rot;
            double a1 = 2.0 * M_PI * (j + W) / dirs + rot;
            double cyv = (-rbuf[j + W] * std::cos(a1)) - (-rbuf[j - W] * std::cos(a0));
            double czv = (-rbuf[j + W] * std::sin(a1)) - (-rbuf[j - W] * std::sin(a0));
            double cn = std::hypot(cyv, czv);
            if (cn < 1e-9) continue;
            double aj = 2.0 * M_PI * j / dirs + rot;
            double dotv = std::fabs(cyv * (-std::cos(aj)) + czv * (-std::sin(aj)));
            if (dotv / cn > cosT) flag[j] = 1;       // chord within mixpix_deg
        }
        int run_first = -1, run_last = -1;
        std::vector<int> run; run.reserve(64);
        auto closeRun = [&]() {
            if (run_first >= 0 && run_last - run_first < MIX_RUN_EXEMPT)
                for (int q : run) drop[q] = 1;       // short run = edge trail
            run.clear(); run_first = run_last = -1;
        };
        for (int j = 0; j < dirs; ++j) {
            if (!flag[j]) continue;
            if (run_first >= 0 && j - run_last > MIX_RUN_GAP) closeRun();
            if (run_first < 0) run_first = j;
            run_last = j; run.push_back(j);
        }
        closeRun();
        const std::vector<unsigned char> drop0(drop); // snapshot: single dilation
        for (int j = 0; j < dirs; ++j) {             // enlarge pass
            if (!ok[j] || drop0[j]) continue;
            int nb = 0;
            for (int o : {-2, -1, 1, 2})
                if (j + o >= 0 && j + o < dirs && drop0[j + o]) ++nb;
            if (nb >= 2) drop[j] = 2;
        }
    }

    // pass 3: emit with stride
    int every = m_cfg.print_every > 0 ? m_cfg.print_every : 1;
    long cnt = 0;
    std::vector<float> ys, zs, is;
    for (int j = 0; j < dirs; j += every) {
        if (!ok[j] || drop[j]) continue;
        double ang = 2.0 * M_PI * (double)j / (double)dirs + rot;
        double yy = -rbuf[j] * std::cos(ang), zz = -rbuf[j] * std::sin(ang);
        if (m_onProfile) {
            ys.push_back((float)yy); zs.push_back((float)zz); is.push_back((float)abuf[j]);
        } else {
            printf("%9.3f %9.3f %9.3f %9.3f %9.0f\n", t_s, x, yy, zz, abuf[j]);
        }
        ++cnt;
    }
    if (m_onProfile && cnt) m_onProfile(t_s, ys, zs, is);
    return cnt;
}

// --------------------------------------------------------------------------- //
// generic helpers
// --------------------------------------------------------------------------- //
bool ZnF9020::setNonBlocking(int fd) {
    int fl = fcntl(fd, F_GETFL, 0);
    return fl >= 0 && fcntl(fd, F_SETFL, fl | O_NONBLOCK) == 0;
}

// wait until fd is ready for `events` (POLLIN/POLLOUT); false on timeout.
// POLLERR/POLLHUP also count as ready so the next recv()/send() surfaces it.
bool ZnF9020::waitFd(int fd, short events, double timeout_s) {
    pollfd p{}; p.fd = fd; p.events = events;
    int ms = (int)(timeout_s * 1000.0 + 0.5);
    for (;;) {
        int rc = poll(&p, 1, ms);
        if (rc > 0)  return true;
        if (rc == 0) return false;                 // timeout
        if (errno != EINTR) return false;
    }
}

// read exactly n bytes from a non-blocking fd; fails if the peer closes or no
// byte arrives within idle_timeout_s (same semantics as the old SO_RCVTIMEO).
bool ZnF9020::recvExact(int fd, void* buf, size_t n, double idle_timeout_s) {
    unsigned char* p = (unsigned char*)buf; size_t got = 0;
    while (got < n) {
        ssize_t r = recv(fd, p + got, n - got, 0);
        if (r > 0) { got += (size_t)r; continue; }
        if (r == 0) return false;                              // peer closed
        if (errno == EINTR) continue;
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            if (!waitFd(fd, POLLIN, idle_timeout_s)) return false;
            continue;
        }
        return false;
    }
    return true;
}

// write all bytes to a non-blocking fd within an overall deadline.
bool ZnF9020::sendAll(int fd, const std::string& s, double timeout_s) {
    size_t sent = 0;
    const double deadline = nowSeconds() + timeout_s;
    while (sent < s.size()) {
        ssize_t r = send(fd, s.data() + sent, s.size() - sent, MSG_NOSIGNAL);
        if (r > 0) { sent += (size_t)r; continue; }
        if (r < 0 && errno == EINTR) continue;
        if (r < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            const double left = deadline - nowSeconds();
            if (left <= 0.0 || !waitFd(fd, POLLOUT, left)) return false;
            continue;
        }
        return false;
    }
    return true;
}

int ZnF9020::connectTcp(const std::string& ip, int port, double timeout_s) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    sockaddr_in a{}; a.sin_family = AF_INET; a.sin_port = htons((uint16_t)port);
    if (inet_pton(AF_INET, ip.c_str(), &a.sin_addr) != 1 || !setNonBlocking(fd)) {
        close(fd); return -1;
    }
    int rc = ::connect(fd, (sockaddr*)&a, sizeof(a));
    if (rc < 0 && errno != EINPROGRESS) { close(fd); return -1; }
    if (rc < 0) {                                  // connection in progress
        if (!waitFd(fd, POLLOUT, timeout_s)) { close(fd); return -1; }
        int err = 0; socklen_t el = sizeof(err);
        getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &el);
        if (err) { close(fd); return -1; }
    }
    return fd;                                     // socket stays non-blocking
}

std::string ZnF9020::localIp(const std::string& ip) {
    int s = socket(AF_INET, SOCK_DGRAM, 0); std::string out = "127.0.0.1";
    if (s < 0) return out;
    sockaddr_in a{}; a.sin_family = AF_INET; a.sin_port = htons(PORT_COMMAND);
    inet_pton(AF_INET, ip.c_str(), &a.sin_addr);
    if (::connect(s, (sockaddr*)&a, sizeof(a)) == 0) {
        sockaddr_in la{}; socklen_t ll = sizeof(la);
        if (getsockname(s, (sockaddr*)&la, &ll) == 0) {
            char b[64]; inet_ntop(AF_INET, &la.sin_addr, b, sizeof(b)); out = b;
        }
    }
    close(s); return out;
}

uint32_t ZnF9020::rdU32(const unsigned char* p) {
    return (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24);
}

int ZnF9020::parseHeaderPixel(const unsigned char* d, size_t n) {
    size_t pos = 8;
    while (pos + 4 <= n) {
        uint16_t tag = (uint16_t)(d[pos] | (d[pos+1] << 8));
        unsigned len = d[pos+2], typ = d[pos+3];
        if (tag == 0xFFFF) break;
        if (typ == 0x01) { pos += 16 + ((len + 15) / 16) * 16; continue; }
        if (typ == 0x00 && (len==1||len==2||len==4||len==8||len==9||len==12||len==16)) {
            if (tag == 0x0002 && len == 4) return (int)rdU32(d + pos + 4);
            pos += ((4 + len + 15) / 16) * 16; continue;
        }
        break;
    }
    return -1;
}

size_t ZnF9020::inflateLine(const unsigned char* in, size_t inlen, std::vector<unsigned char>& out) {
    // RAII: inflateEnd() bound to scope, so no exit path can leak the stream
    struct InflateStream {
        z_stream zs{};
        const bool ok;
        InflateStream() : ok(inflateInit2(&zs, 15) == Z_OK) {}
        ~InflateStream() { if (ok) inflateEnd(&zs); }
        InflateStream(const InflateStream&)            = delete;
        InflateStream& operator=(const InflateStream&) = delete;
        InflateStream(InflateStream&&)                 = delete;
        InflateStream& operator=(InflateStream&&)      = delete;
    } z;
    if (!z.ok) return 0;
    if (out.size() < (1u << 20)) out.resize(1u << 20);
    z.zs.next_in = (Bytef*)in; z.zs.avail_in = (uInt)inlen;
    z.zs.next_out = out.data(); z.zs.avail_out = (uInt)out.size();
    inflate(&z.zs, Z_SYNC_FLUSH);
    return z.zs.total_out;
}

bool ZnF9020::statusInt(const std::string& xml, long& out) {
    size_t p = xml.find("<status>");
    if (p == std::string::npos) return false;
    out = strtol(xml.c_str() + p + 8, nullptr, 10);
    return true;
}

std::string ZnF9020::affirmative(const std::string& xml) {
    size_t p = xml.find("act=\"");
    if (p == std::string::npos) return "yes";
    p += 5; size_t e = p;
    while (e < xml.size() && xml[e] != ',' && xml[e] != '"') ++e;
    std::string a = xml.substr(p, e - p);
    return a.empty() ? "yes" : a;
}

std::string ZnF9020::statusStr(long s) {
    std::string r;
    auto add = [&](int bit, const char* nm){ if (s & bit) { if(!r.empty()) r+=", "; r+=nm; } };
    add(1<<0,"ID"); add(1<<1,"STREAMING"); add(1<<2,"SCANNING"); add(1<<3,"ERROR");
    add(1<<4,"WRITING"); add(1<<5,"PIN"); add(1<<6,"LOCKED"); add(1<<8,"NOT-READY");
    add(1<<11,"DATA-INVALID"); add(1<<15,"STARTUP");
    char b[160]; snprintf(b, sizeof(b), "0x%lx [%s]", s, r.empty() ? "none" : r.c_str());
    return b;
}
