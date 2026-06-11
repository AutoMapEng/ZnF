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

static double now_s() {
    using clk = std::chrono::steady_clock;
    return std::chrono::duration<double>(clk::now().time_since_epoch()).count();
}
static void sleep_s(double s) {
    std::this_thread::sleep_for(std::chrono::duration<double>(s));
}

// poll() timeouts: while idle between frames we wake every MARKER_TIMEOUT_S so
// streamFor() can check its deadline; a stall in the MIDDLE of a frame or while
// sending a command is treated as an error/failure instead of waiting forever.
static constexpr double MARKER_TIMEOUT_S     = 1.0;
static constexpr double FRAME_IDLE_TIMEOUT_S = 3.0;
static constexpr double SEND_TIMEOUT_S       = 5.0;

// --------------------------------------------------------------------------- //
// lifecycle
// --------------------------------------------------------------------------- //
ZnF9020::ZnF9020(const Config& cfg) : cfg_(cfg) {}
ZnF9020::~ZnF9020() { stop(); disconnect(); }

bool ZnF9020::start() {
    local_ = cfg_.local_ip.empty() ? localIp(cfg_.ip) : cfg_.local_ip;
    scan_cmd_ = buildScanCommand();              // built once (unique file name)
    fprintf(stderr, "# connecting to scanner %s (command :%d, stream :%d)\n",
            cfg_.ip.c_str(), PORT_COMMAND, PORT_STREAM);
    fprintf(stderr, "# local ip advertised: %s\n", local_.c_str());
    fprintf(stderr, "# scan command: %s\n", scan_cmd_.c_str());

    if (!connect()) {
        fprintf(stderr, "!! cannot reach scanner command port %s:%d\n",
                cfg_.ip.c_str(), PORT_COMMAND);
        if (local_ == cfg_.ip)
            fprintf(stderr, "   (that is THIS PC's own IP - use the scanner's address)\n");
        return false;
    }
    if (!startScan()) { stop(); return false; }

    if (!on_profile_)
        printf("# t[s]      x[m]      y[m]      z[m]   intensity\n");
    streamFor(cfg_.seconds);
    stop();
    return true;
}

// --------------------------------------------------------------------------- //
// command channel
// --------------------------------------------------------------------------- //
bool ZnF9020::connect() {
    cmd_fd_ = connectTcp(cfg_.ip, PORT_COMMAND);
    return cmd_fd_ >= 0;
}

std::string ZnF9020::command(const std::string& text, double wait) {
    if (cmd_fd_ < 0) return "";
    if (!sendAll(cmd_fd_, text + "\r\n", SEND_TIMEOUT_S)) return "";
    std::string reply;
    char buf[4096];
    for (;;) {
        if (!waitFd(cmd_fd_, POLLIN, wait)) break;       // no (more) data in time
        ssize_t r = recv(cmd_fd_, buf, sizeof(buf), 0);
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

void ZnF9020::ensureIdle(double wait) {
    const long BUSY = (1<<1)|(1<<2)|(1<<3)|(1<<15);   // STREAM|SCAN|ERR|STARTUP
    std::string st;
    for (int i = 0; i < 12; ++i) {
        st = command("status", wait);
        if (st.find("<messagebox") != std::string::npos) {
            command(affirmative(st), wait); sleep_s(0.3); continue;
        }
        long s = 0;
        if (statusInt(st, s) && (s & BUSY)) { command("stop", wait); sleep_s(0.5); continue; }
        break;
    }
    long s = 0;
    fprintf(stderr, "# status (idle): %s\n",
            statusInt(st, s) ? statusStr(s).c_str() : st.c_str());
}

// --------------------------------------------------------------------------- //
// scanning / streaming
// --------------------------------------------------------------------------- //
bool ZnF9020::startScan() {
    prev_r_.clear(); warm_n_ = 0;            // fresh temporal state per scan
    fprintf(stderr, "# version: %.80s ...\n", command("version").c_str());
    ensureIdle();
    sleep_s(0.5);
    fprintf(stderr, "# scan reply: %s\n", command(scan_cmd_).c_str());
    scanning_ = true;
    for (int i = 0; i < 10 && !abort_.load(); ++i) { // auto-answer overwrite/messagebox
        sleep_s(0.5);
        std::string st = command("status");
        if (st.find("<messagebox") != std::string::npos) {
            fprintf(stderr, "# messagebox -> answering '%s'\n", affirmative(st).c_str());
            command(affirmative(st));
            continue;
        }
        long s = 0;
        fprintf(stderr, "# status (after): %s\n",
                statusInt(st, s) ? statusStr(s).c_str() : st.c_str());
        break;
    }
    double deadline = now_s() + 10;
    while (stream_fd_ < 0 && now_s() < deadline && !abort_.load()) {
        stream_fd_ = connectTcp(cfg_.ip, PORT_STREAM, 2.0);
        if (stream_fd_ < 0) sleep_s(0.5);
    }
    if (stream_fd_ < 0) { fprintf(stderr, "!! could not connect to stream port\n"); return false; }
    int rcvbuf = 4 * 1024 * 1024;           // absorb GUI/X11 stalls without
    setsockopt(stream_fd_, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));
    return true;
}

void ZnF9020::streamFor(double seconds) {
    if (stream_fd_ < 0) return;
    double t_s = 0.0; long line_no = -1;
    bool got_data = false;
    double deadline = now_s() + seconds + 30.0;  // provisional: allow ~12 s spin-up
    double last_stat = now_s();
    fprintf(stderr, "# waiting for scanner spin-up (data-invalid phase) ...\n");
    while (now_s() < deadline && !abort_.load()) {
        uint16_t type;
        FrameRes fr = readFrame(type, payload_);
        if (fr == FRAME_TIMEOUT) continue;
        if (fr == FRAME_ERR) { fprintf(stderr, "!! stream error\n"); break; }

        if (type == ZFS_HEADER_ID) {
            int p = parseHeaderPixel(payload_.data(), payload_.size());
            if (p > 0) { pixel_ = p; fprintf(stderr, "# zfs header: pixel/line = %d\n", pixel_); }
        } else if (type == LINE_HEADER_ID) {
            lineheader_ = payload_;
            t_s     = (lineheader_.size() >= 32) ? rdU32(lineheader_.data() + 28) / 1000.0 : 0.0;
            line_no = (lineheader_.size() >= 36) ? (long)rdU32(lineheader_.data() + 32) : -1;
        } else if (type == RAW_PIXEL_ID || type == COMP_PIXEL_ID) {
            const unsigned char* data = payload_.data();
            size_t n = payload_.size();
            if (type == COMP_PIXEL_ID) {
                n = inflateLine(payload_.data(), payload_.size(), inflated_);
                data = inflated_.data();
                if (n == 0) continue;
            }
            if (pixel_ < 0) pixel_ = (int)(n / 8);
            long li = (line_no >= 0) ? line_no : lines_;
            long got = decodeProfile(data, n, t_s, (double)li * cfg_.spacing);
            pts_ += got;
            ++lines_;
            if (!got_data && got > 0) {
                got_data = true;
                deadline = now_s() + seconds;    // --seconds counts from first data
                fprintf(stderr, "# first valid data - %.0f s acquisition window starts\n", seconds);
            }
        }
        if (now_s() - last_stat >= 1.0) {
            fprintf(stderr, "# ... %ld lines, %ld points\n", lines_, pts_);
            last_stat = now_s();
        }
    }
    fprintf(stderr, "# acquired ~%ld lines, %ld points\n", lines_, pts_);
}

void ZnF9020::stop() {
    if (scanning_ && cmd_fd_ >= 0)
        fprintf(stderr, "# stop reply: %s\n", command("stop").c_str());
    scanning_ = false;
}

void ZnF9020::disconnect() {
    if (stream_fd_ >= 0) { close(stream_fd_); stream_fd_ = -1; }
    if (cmd_fd_ >= 0)    { close(cmd_fd_);    cmd_fd_ = -1; }
}

// --------------------------------------------------------------------------- //
// scan-command building, framing, decode
// --------------------------------------------------------------------------- //
std::string ZnF9020::buildScanCommand() const {
    if (!cfg_.scan_args.empty()) return "scan " + cfg_.scan_args;
    std::string fname = cfg_.file;
    if (fname.empty()) {
        char ts[32]; time_t t = time(nullptr); tm lt; localtime_r(&t, &lt);
        strftime(ts, sizeof(ts), "live_%Y%m%d-%H%M%S", &lt);
        fname = ts;
    }
    std::string cmd = "scan -profiler -resolution " + cfg_.resolution +
                      " -quality " + cfg_.quality + " -file " + fname +
                      " -path scans -filesize 250mb -linestream " + cfg_.mode +
                      " -statusstream -memorize 3 -writing 0 -ip " + local_;
    char num[64];
    if (cfg_.has_rps) { snprintf(num, sizeof(num), " %s %g", RPS_FLAG, cfg_.rps); cmd += num; }
    if (cfg_.has_mhz) { snprintf(num, sizeof(num), " %s %g", MHZ_FLAG, cfg_.mhz); cmd += num; }
    if (!cfg_.extra.empty()) cmd += " " + cfg_.extra;
    return cmd;
}

ZnF9020::FrameRes ZnF9020::readFrame(uint16_t& type, std::vector<unsigned char>& payload) {
    unsigned char m[4];
    if (!recvExact(stream_fd_, m, 4, MARKER_TIMEOUT_S)) return FRAME_TIMEOUT;
    while (!(m[0]=='Z' && m[1]=='+' && m[2]=='F' && m[3]==0)) {      // resync to marker
        m[0]=m[1]; m[1]=m[2]; m[2]=m[3];
        if (!recvExact(stream_fd_, &m[3], 1, MARKER_TIMEOUT_S)) return FRAME_TIMEOUT;
    }
    unsigned char h[10];
    if (!recvExact(stream_fd_, h, 10, FRAME_IDLE_TIMEOUT_S)) return FRAME_ERR;
    type = (uint16_t)(h[0] | (h[1] << 8));
    uint32_t size = (uint32_t)h[2] | ((uint32_t)h[3]<<8) | ((uint32_t)h[4]<<16) | ((uint32_t)h[5]<<24);
    payload.resize(size);
    if (size && !recvExact(stream_fd_, payload.data(), size, FRAME_IDLE_TIMEOUT_S)) return FRAME_ERR;
    return FRAME_OK;
}

long ZnF9020::decodeProfile(const unsigned char* data, size_t n, double t_s, double x) {
    const uint16_t* u = (const uint16_t*)data;       // host is LE
    if (pixel_ <= 0 || n < (size_t)pixel_ * 8) return 0;
    // Layout: pixel_/2 directions. plane0[half+j] is the range channel; in the
    // live tcpzip stream it is COARSE-quantised (~25.7 mm: high byte duplicated
    // into the low byte) and wraps every 6.5536 m. plane1[half+j] = amplitude.
    // plane2[j] (x0.2 mm, caps 13.1 m) is an independent noisy (+/-0.3 m)
    // estimate that resolves the wrap count k - but on weak/grazing returns it
    // "parks" on repeated code values instead of measuring (a parked plane2
    // exactly repeats its raw value across neighbours; a real one jitters).
    // Nonius decode (cf. SDK Nonius/noniusjump): k from plane2 where it
    // measures (anchors, validated by local k-consensus), k by continuity
    // along the profile elsewhere (chains grown from the anchors).
    // No-return STATUS pixels have the duplicated-byte code in plane0 AND
    // plane2 == the same raw value.
    static constexpr double AMBIG_M     = 6.5536;
    static constexpr double SENTINEL_M  = 0.345;  // window/no-return zone
    static constexpr double SEC_MIN_M   = 0.40;   // plane2 below this = code
    static constexpr double SEC_TOL_M   = 1.2;    // anchor: |plane2 - r| limit
    static constexpr int    PARKED_NB   = 4;      // +/- neighbours checked
    static constexpr int    PARKED_MIN  = 3;      // exact repeats => parked
    static constexpr int    CONS_WIN    = 16;     // consensus window [px]
    static constexpr int    CONS_MIN    = 3;      // min anchors in window
    static constexpr double CONS_FRAC   = 0.6;    // same-k fraction required
    static constexpr int    GAP_STRICT  = 24;     // plain chain max gap [px]
    static constexpr int    GAP_BRIDGE  = 96;     // validated-bridge max gap
    static constexpr int    BRIDGE_MIN  = 12;     // px to validate a bridge
    static constexpr int    BRIDGE_DIV  = 4;      // distinct raws to validate
    static constexpr double BRIDGE_ENTRY_TOL = 0.70; // bridge must CONTINUE the
                                                  // surface: a depth jump at a
                                                  // long gap may not start one
                                                  // (folded-wall ghosts did)
    static constexpr double CHAIN_TOL_M = 1.5;    // chain tolerance cap
    static constexpr double SLOPE_MAX   = 0.35;   // chain slope cap [m/px]
    static constexpr double TEMPORAL_TOL = 0.40;  // chained px must match the
                                                  // previous profile (+/-1 px)
                                                  // to be emitted (anchors are
                                                  // exempt; kills k-flicker)
    const int dirs = pixel_ / 2;
    const uint16_t* pf = u + dirs;
    const uint16_t* pa = u + pixel_ + dirs;
    const uint16_t* pc = u + 2 * pixel_;
    const double rot = cfg_.rotate_deg * M_PI / 180.0;
    auto dup = [](uint16_t v) { return (v & 0xFF) == (v >> 8); };

    // pass 1: per-pixel decode, plane2-anchor selection
    std::vector<double> rbuf(dirs, 0.0), abuf(dirs, 0.0), fine(dirs), sec(dirs);
    std::vector<unsigned char> real(dirs, 0), anch(dirs, 0);
    std::vector<signed char>   ksec(dirs, 0);
    for (int j = 0; j < dirs; ++j) {
        abuf[j] = pa[j];
        fine[j] = pf[j] * RANGE_SCALE_M;
        sec[j]  = pc[j] * 2.0 * RANGE_SCALE_M;
        if (dup(pf[j]) && pc[j] == pf[j]) continue;  // no-return status pixel
        if (abuf[j] <= cfg_.min_amp) continue;
        real[j] = 1;
    }
    std::vector<unsigned char> seeded(dirs, 0);      // plane2-verified px
    int anchors_n = dirs, real_n = 0;                // plane2 anchors this line
    for (int j = 0; j < dirs; ++j) real_n += real[j];
    if (!cfg_.unwrap) {                              // fold at 6.55 m, no plane2
        for (int j = 0; j < dirs; ++j)
            if (real[j] && fine[j] >= SENTINEL_M) { rbuf[j] = fine[j]; seeded[j] = 1; }
    } else {
        std::vector<unsigned char> secmeas(dirs, 0); // plane2 actually measuring
        std::vector<int> ai; ai.reserve(dirs / 2);
        for (int j = 0; j < dirs; ++j) {
            int parked = 0;                          // exact plane2 repeats
            for (int o = -PARKED_NB; o <= PARKED_NB; ++o) {
                if (!o) continue;
                if (pc[(j + o + dirs) % dirs] == pc[j]) ++parked;
            }
            secmeas[j] = (parked < PARKED_MIN && sec[j] >= SEC_MIN_M);
            if (!real[j] || !secmeas[j]) continue;
            double kk = std::round((sec[j] - fine[j]) / AMBIG_M);
            if (kk < 0) kk = 0; else if (kk > 2) kk = 2;
            double r = fine[j] + kk * AMBIG_M;
            if (std::fabs(sec[j] - r) > SEC_TOL_M) continue;
            if (kk == 0 && fine[j] < SENTINEL_M) continue;
            ksec[j] = (signed char)kk;
            anch[j] = 1; ai.push_back(j);
        }
        // consensus: an anchor seeds chains only if its k agrees with the
        // neighbourhood (lone garbage anchors would seed fold ghosts)
        anchors_n = (int)ai.size();
        const int na = (int)ai.size();
        for (int t = 0, lo = 0, hi = 0; t < na; ++t) {
            while (lo < na && ai[lo] < ai[t] - CONS_WIN) ++lo;
            while (hi < na && ai[hi] <= ai[t] + CONS_WIN) ++hi;
            int same = 0;
            for (int q = lo; q < hi; ++q) if (ksec[ai[q]] == ksec[ai[t]]) ++same;
            if (hi - lo >= CONS_MIN && (double)same >= CONS_FRAC * (hi - lo)) {
                int jj = ai[t];
                // wrap-trap demotion: a k=0 seed (plane2 ~ plane0 carries no
                // wrap info) on a direction already locked at k>=1 yields to
                // the lock - the hysteresis pass will paint it at k>=1.
                if (ksec[jj] == 0 && (int)prev_r_.size() == dirs &&
                    warm_n_ >= 8 && prev_r_[jj] > 0.0f) {
                    // exact-j only: reading NEIGHBOUR locks here lets a k>=1
                    // region alias-creep over adjacent true-k0 seeds (ratchet)
                    double kp = std::round(((double)prev_r_[jj] - fine[jj]) / AMBIG_M);
                    if (kp >= 1 && kp <= 2 &&
                        std::fabs(fine[jj] + kp * AMBIG_M - (double)prev_r_[jj])
                            <= TEMPORAL_TOL)
                        continue;
                }
                rbuf[jj] = fine[jj] + ksec[jj] * AMBIG_M;
                seeded[jj] = 1;
            }
        }
        // plane2 veto: a chained pixel whose plane2 MEASURES (jitters, >=0.4 m)
        // yet matches NO wrap alias of plane0 is a mixed pixel (plane2 locked
        // a different echo) - paint it at no k. A plane2 that matches some
        // alias merely cannot RESOLVE k (it may wrap like plane0) - no veto.
        auto secveto = [&](int j, double) {
            if (!secmeas[j]) return false;
            double d = std::fabs(sec[j] - fine[j]);
            for (int kq = 1; kq <= 2; ++kq)
                d = std::min(d, std::fabs(sec[j] - fine[j] - kq * AMBIG_M));
            return d > 2.5;
        };
        // previous-profile k hysteresis: once a direction's wrap count is
        // established (e.g. the far wall painted at k=1 via the ceiling-corner
        // chain), it sticks across profiles - an alias slide (a folded surface
        // numerically continuing a closer one, e.g. chair -> wrapped wall at
        // k=0) cannot re-claim the sector on later profiles.
        std::vector<unsigned char> prevpass(dirs, 0);
        if ((int)prev_r_.size() == dirs && warm_n_ >= 8) {
            for (int j = 0; j < dirs; ++j) {
                if (rbuf[j] > 0.0 || !real[j] || prev_r_[j] <= 0.0f) continue;
                double kp = std::round(((double)prev_r_[j] - fine[j]) / AMBIG_M);
                if (kp < 1 || kp > 2) continue;  // never lock k=0: the fold
                                                 // alias is k=0 by construction
                                                 // and close content re-anchors
                                                 // via plane2 every profile
                double cand = fine[j] + kp * AMBIG_M;
                if (std::fabs(cand - (double)prev_r_[j]) > TEMPORAL_TOL) continue;
                if (kp == 0 && fine[j] < SENTINEL_M) continue;
                if (secveto(j, cand)) continue;
                rbuf[j] = cand; prevpass[j] = 1;     // chained (not seeded)
            }
        }
        // continuity chains (both sweep directions): resolve k for pixels whose
        // plane2 parked, by predicting r from the running surface; long gaps
        // are bridged only when a whole run validates (length + raw diversity)
        struct Pend { int j; double r; uint16_t fraw; unsigned char isAnchor; };
        std::vector<Pend> pend; pend.reserve(GAP_BRIDGE);
        for (int sweep = 0; sweep < 2; ++sweep) {
            long last_j = -(1L << 30), prev_j = -(1L << 30);
            double last_r = 0.0, prev_r = 0.0;
            bool bridging = false; pend.clear();
            // fold-crossing credential: only a chain that has ITSELF stepped
            // k by +/-1 across a tight gap (the genuine wrap-crossing
            // signature) may re-key seeded pixels. Chains that merely ride
            // pre-painted state (captures) never earn it - otherwise locked
            // junk ratchets the whole room up one wrap, profile by profile.
            bool cred = false; int last_k = 0;
            for (int t = 0; t < dirs; ++t) {
                int i = sweep ? dirs - 1 - t : t;
                if (rbuf[i] > 0.0) {
                    // chain-context override: plane2's wrap ambiguity can seed
                    // k=0 on FOLDED content (it wraps like plane0: sec 0.93 ~
                    // fine 0.90 for a surface truly at 7.45 m). A live chain
                    // carrying the true surface bumps such a pixel one wrap up
                    // when k+1 fits it tightly and the k=0 reading does not.
                    // A pixel that CONFLICTS with the live chain but cannot be
                    // re-keyed is transparent: it must not capture the chain
                    // (one veto-blocked pixel would hand the whole downstream
                    // sector to the fold alias).
                    bool ovr = cred &&
                               ((seeded[i] && ksec[i] == 0) || prevpass[i]) &&
                               rbuf[i] - fine[i] < 0.5 * AMBIG_M;
                    if (ovr && last_j > -(1L << 29) &&
                        std::labs(i - last_j) <= GAP_STRICT) {
                        long gap0 = std::labs(i - last_j);
                        double slope0 = 0.0;
                        long dj0 = last_j - prev_j;
                        if (dj0 != 0 && std::labs(dj0) <= 4) {
                            slope0 = (last_r - prev_r) / (double)dj0;
                            if (slope0 > SLOPE_MAX) slope0 = SLOPE_MAX;
                            if (slope0 < -SLOPE_MAX) slope0 = -SLOPE_MAX;
                        }
                        double pred0 = last_r + slope0 * (double)(i - last_j);
                        double tol0 = 0.25 + 0.05 * (double)gap0;
                        if (tol0 > CHAIN_TOL_M) tol0 = CHAIN_TOL_M;
                        double up = rbuf[i] + AMBIG_M;
                        if (std::fabs(up - pred0) <= tol0 &&
                            std::fabs(rbuf[i] - pred0) > tol0 &&
                            !secveto(i, up)) {
                            rbuf[i] = up;                // re-keyed one wrap up
                            seeded[i] = 0; prevpass[i] = 0;  // chain-grade now
                        } else if (std::fabs(rbuf[i] - pred0) > tol0) {
                            continue;                    // conflicting: transparent
                        }
                    }
                    if (bridging) { pend.clear(); bridging = false; }
                    {   double kr = std::round((rbuf[i] - fine[i]) / AMBIG_M);
                        last_k = kr < 0 ? 0 : (kr > 2 ? 2 : (int)kr); }
                    prev_j = last_j; prev_r = last_r; last_j = i; last_r = rbuf[i];
                    continue;                        // captures never grant cred
                }
                if (!real[i]) continue;
                long gap = std::labs(i - last_j);
                if (gap > GAP_BRIDGE) continue;
                double slope = 0.0;
                long dj = last_j - prev_j;
                if (dj != 0 && std::labs(dj) <= 4) {
                    slope = (last_r - prev_r) / (double)dj;
                    if (slope > SLOPE_MAX) slope = SLOPE_MAX;
                    if (slope < -SLOPE_MAX) slope = -SLOPE_MAX;
                }
                double pred = last_r + slope * (double)(i - last_j);
                double kk = std::round((pred - fine[i]) / AMBIG_M);
                if (kk < 0) kk = 0; else if (kk > 2) kk = 2;
                double cand = fine[i] + kk * AMBIG_M;
                double tol = 0.25 + 0.05 * (double)gap;
                if (tol > CHAIN_TOL_M) tol = CHAIN_TOL_M;
                if (std::fabs(cand - pred) <= tol &&
                    !(kk == 0 && fine[i] < SENTINEL_M) && !secveto(i, cand)) {
                    if (gap > GAP_STRICT && !bridging) {
                        if (std::fabs(cand - pred) > BRIDGE_ENTRY_TOL) continue;
                        bridging = true; pend.clear();
                        pend.push_back({i, cand, pf[i], anch[i]});
                    } else if (bridging) {
                        pend.push_back({i, cand, pf[i], anch[i]});
                        if ((int)pend.size() >= BRIDGE_MIN) {
                            int div = 0; bool hasA = false;
                            for (size_t q = 0; q < pend.size(); ++q) {
                                hasA = hasA || pend[q].isAnchor;
                                bool seen = false;
                                for (size_t w = 0; w < q; ++w)
                                    if (pend[w].fraw == pend[q].fraw) { seen = true; break; }
                                if (!seen) ++div;
                            }
                            if (div >= BRIDGE_DIV || hasA) {
                                for (const Pend& pd : pend) rbuf[pd.j] = pd.r;
                                pend.clear(); bridging = false;
                            }
                        }
                    } else {
                        rbuf[i] = cand;
                    }
                    if (gap <= 8 && last_j > -(1L << 29) &&
                        std::abs((int)kk - last_k) == 1)
                        cred = true;                 // genuine fold crossing
                    last_k = (int)kk;
                    prev_j = last_j; prev_r = last_r; last_j = i; last_r = cand;
                } else if (bridging) {
                    pend.clear(); bridging = false;
                    last_j = -(1L << 30); cred = false; // restart from next seed
                }
            }
        }
    }
    std::vector<unsigned char> ok(dirs, 0);
    const bool warm = (int)prev_r_.size() == dirs;
    std::vector<float> cur(dirs, 0.0f);
    for (int j = 0; j < dirs; ++j) {
        if (rbuf[j] <= 0.0) continue;
        if (rbuf[j] > 200.0) continue;               // spec max-range guard
        if (rbuf[j] < cfg_.min_range_m) continue;
        ok[j] = 1; cur[j] = (float)rbuf[j];
    }
    // temporal confirmation: a CHAINED pixel (no plane2 verification) is only
    // emitted once the previous profile agrees at the same direction (+/-1 px).
    // One-profile warmup for new structures; kills per-profile k-flicker.
    for (int j = 0; j < dirs; ++j) {
        if (!ok[j] || seeded[j]) continue;
        bool conf = false;
        if (warm)
            for (int o = -1; o <= 1 && !conf; ++o) {
                int q = j + o;
                if (q >= 0 && q < dirs && prev_r_[q] > 0.0f &&
                    std::fabs(prev_r_[q] - rbuf[j]) <= TEMPORAL_TOL) conf = true;
            }
        if (!conf) ok[j] = 0;
    }
    // fold-corner fuzz: a plane2-seeded k=0 pixel whose NEIGHBOUR direction
    // is locked at k>=1 consistent with this pixel's fine is wrap-ambiguous -
    // suppress its EMISSION only (the state keeps the full decode: no
    // feedback, so unlike seed demotion at +/-1 this cannot alias-creep).
    if (cfg_.unwrap && warm && warm_n_ >= 8) {
        for (int j = 0; j < dirs; ++j) {
            if (!ok[j] || !seeded[j] || ksec[j] != 0) continue;
            bool amb = false;
            for (int o = -1; o <= 1 && !amb; ++o) {
                int q = j + o;
                if (q < 0 || q >= dirs || prev_r_[q] <= 0.0f) continue;
                double kp = std::round(((double)prev_r_[q] - fine[j]) / AMBIG_M);
                if (kp >= 1 && kp <= 2 &&
                    std::fabs(fine[j] + kp * AMBIG_M - (double)prev_r_[q])
                        <= TEMPORAL_TOL)
                    amb = true;
            }
            if (amb) ok[j] = 0;
        }
    }
    // spin-up/transition junk (plane2 globally dead: laser on but anchors a
    // tiny FRACTION of real pixels - healthy lines run ~25-60%) must not
    // poison the temporal state; otherwise remember the FULL decode
    // (pre-suppression) so new stable structures confirm on the next profile
    if (anchors_n >= 16 && 10 * anchors_n >= real_n) {
        prev_r_.swap(cur);
        ++warm_n_;            // hysteresis activates after 8 healthy profiles
    }

    // pass 2: mixed-pixel (edge-trail) filter - drop points whose local chord
    // is nearly parallel to the beam, EXCEPT long runs of such points (a real
    // surface seen at grazing incidence, e.g. the far floor, is also chord-
    // radial but spans far more pixels than a spot-size-bounded edge trail).
    std::vector<unsigned char> drop(dirs, 0);
    if (cfg_.mixpix_deg > 0.0) {
        static constexpr int MIX_RUN_EXEMPT = 32;    // px span = real surface
        static constexpr int MIX_RUN_GAP    = 8;     // px joining run members
        const int W = 4;
        const double cosT = std::cos(cfg_.mixpix_deg * M_PI / 180.0);
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
    int every = cfg_.print_every > 0 ? cfg_.print_every : 1;
    long cnt = 0;
    std::vector<float> ys, zs, is;
    for (int j = 0; j < dirs; j += every) {
        if (!ok[j] || drop[j]) continue;
        double ang = 2.0 * M_PI * (double)j / (double)dirs + rot;
        double yy = -rbuf[j] * std::cos(ang), zz = -rbuf[j] * std::sin(ang);
        if (on_profile_) {
            ys.push_back((float)yy); zs.push_back((float)zz); is.push_back((float)abuf[j]);
        } else {
            printf("%9.3f %9.3f %9.3f %9.3f %9.0f\n", t_s, x, yy, zz, abuf[j]);
        }
        ++cnt;
    }
    if (on_profile_ && cnt) on_profile_(t_s, ys, zs, is);
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
    const double deadline = now_s() + timeout_s;
    while (sent < s.size()) {
        ssize_t r = send(fd, s.data() + sent, s.size() - sent, MSG_NOSIGNAL);
        if (r > 0) { sent += (size_t)r; continue; }
        if (r < 0 && errno == EINTR) continue;
        if (r < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            const double left = deadline - now_s();
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
    z_stream zs{};
    if (inflateInit2(&zs, 15) != Z_OK) return 0;
    if (out.size() < (1u << 20)) out.resize(1u << 20);
    zs.next_in = (Bytef*)in; zs.avail_in = (uInt)inlen;
    zs.next_out = out.data(); zs.avail_out = (uInt)out.size();
    inflate(&zs, Z_SYNC_FLUSH);
    size_t produced = zs.total_out;
    inflateEnd(&zs);
    return produced;
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
