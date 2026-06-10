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
    return true;
}

void ZnF9020::streamFor(double seconds) {
    if (stream_fd_ < 0) return;
    double t_s = 0.0; long line_no = -1;
    double deadline = now_s() + seconds, last_stat = now_s();
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
            pts_ += decodeProfile(data, n, t_s, (double)li * cfg_.spacing);
            ++lines_;
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

long ZnF9020::decodeProfile(const unsigned char* data, size_t n, double t_s, double x) const {
    const uint16_t* u = (const uint16_t*)data;       // host is LE
    if (pixel_ <= 0 || n < (size_t)pixel_ * 4) return 0;
    const uint16_t* rng = u;                          // ch0 = range
    const uint16_t* amp = u + pixel_;                 // ch1 = amplitude
    int every = cfg_.print_every > 0 ? cfg_.print_every : 1;
    long cnt = 0;
    if (on_profile_) {                                // callback sink (e.g. plotter)
        std::vector<float> ys, zs, is;
        for (int i = 0; i < pixel_; i += every) {
            double a = amp[i], rv = rng[i];
            if (a > cfg_.min_amp && rv > 0.0) {
                double ang = 2.0 * M_PI * (double)i / (double)pixel_;
                double r = rv * RANGE_SCALE_M;
                ys.push_back((float)(r * std::cos(ang)));
                zs.push_back((float)(r * std::sin(ang)));
                is.push_back((float)a);
                ++cnt;
            }
        }
        if (cnt) on_profile_(t_s, ys, zs, is);
        return cnt;
    }
    for (int i = 0; i < pixel_; i += every) {         // default sink: stdout
        double a = amp[i], rv = rng[i];
        if (a > cfg_.min_amp && rv > 0.0) {
            double ang = 2.0 * M_PI * (double)i / (double)pixel_;
            double r = rv * RANGE_SCALE_M;
            printf("%9.3f %9.3f %9.3f %9.3f %9.0f\n",
                   t_s, x, r * std::cos(ang), r * std::sin(ang), a);
            ++cnt;
        }
    }
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
