// ZnF9020.h - controller for a Z+F PROFILER 9020 laser scanner.
//
// Pure-socket driver (no Z+F SDK / license). Talks the device's TCP protocol:
//   * COMMAND  TCP 6100, UTF-8 CRLF-terminated commands, XML replies.
//   * STREAM   TCP 6105, 14-byte "Z+F\0" frames; per-line zlib pixel data.
// Each profile is one vertical-circle revolution; points are emitted as
//   t  x  y  z  intensity   (y,z = cross-section in metres, +Y = -cos, +Z = -sin of mirror angle per Z+F manual, x = synthetic travel).
// Line layout: pixel_/2 directions; all live-stream values byte-doubled
// coarse. plane0[half+j] = range modulo 6.5536 m (0.1 mm units);
// plane1[j] = ABSOLUTE WRAP COUNT k (value/257, 0..27 = full 182.68 m spec);
// plane1[half+j] = amplitude; plane2[j] = secondary estimate used only as a
// mixed-echo sanity check modulo its own 13.107 m interval.
// True range r = plane0 + k*6.5536 - per-pixel absolute, no cross-line state,
// full 182.68 m spec range. Near-field overload garbage (hand <30 cm: ghost
// ARCS during the event, a parked SATURATION SCAR after it) is removed by
// per-pixel gates - see decodeProfile and test_decode.py.
// --no-unwrap ignores k and folds at 6.55 m (debug).
//
// The public header pulls in no socket/zlib headers - those stay in the .cpp.
//
// All socket I/O is NON-BLOCKING: sockets run in O_NONBLOCK mode and every wait
// goes through poll() with an explicit timeout. Public methods still wait
// synchronously for replies/data (the device protocol is sequential), but no
// underlying call can block indefinitely.
#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <functional>
#include <atomic>

class ZnF9020 {
public:
    struct Config {
        std::string ip;
        double      seconds    = 30.0;
        std::string mode       = "tcpzip";   // tcpzip | tcp
        std::string resolution = "10000";
        std::string quality    = "50";
        double      min_amp    = 2000.0;     // drop returns below this intensity
        double      min_range_m = 0.0;        // drop returns closer than this [m]
        bool        unwrap = true;           // gated >6.55 m unwrap (Z+F-like)
        double      mixpix_deg = 8.0;        // mixed-pixel edge-trail filter (0=off)
        double      rotate_deg = 90.8;       // angle-zero/mount correction [deg];
                                             // calibrated 2026-06-10 (dual-mod decode; orientation user-verified)
        double      spacing    = 0.0;        // synthetic travel per profile for X
        int         print_every = 2048;      // print every Nth pixel of a profile
        bool        has_rps = false, has_mhz = false;
        double      rps = 0, mhz = 0;        // motor speed [rev/s] / data rate [MHz]
        std::string file, extra, scan_args, local_ip;
    };

    explicit ZnF9020(const Config& cfg);
    ~ZnF9020();                              // RAII: Stop() + Disconnect()

    // Non-copyable, non-movable: the object owns the socket fds and is shared
    // by address with a worker thread; copies/moves would alias the session.
    ZnF9020(const ZnF9020&)            = delete;
    ZnF9020& operator=(const ZnF9020&) = delete;
    ZnF9020(ZnF9020&&)                 = delete;
    ZnF9020& operator=(ZnF9020&&)      = delete;

    // High-level: connect, start scanning, stream for cfg.seconds, then stop.
    bool Start();

    // Lower-level control interface (usable individually):
    bool        Connect();                              // open command channel (6100)
    std::string Command(const std::string& text, double wait = 2.5);  // send + read XML
    void        EnsureIdle(double wait = 2.5);          // answer boxes / stop running scan
    bool        StartScan();                            // send scan cmd, open data stream
    void        StreamFor(double seconds);              // read frames -> print points
    void        Stop();                                 // stop the scan   (idempotent)
    void        Disconnect();                            // close sockets   (idempotent)

    [[nodiscard]] long Lines()  const noexcept { return m_lines; }
    [[nodiscard]] long Points() const noexcept { return m_pts; }

    // Per-profile callback: when set, decoded points (y,z = cross-section [m],
    // inten = raw amplitude) are delivered here INSTEAD of printed to stdout.
    // Called from the thread running Start()/StreamFor().
    using ProfileCallback = std::function<void(double t_s,
                                               const std::vector<float>& y,
                                               const std::vector<float>& z,
                                               const std::vector<float>& inten)>;
    void SetProfileCallback(ProfileCallback cb) { m_onProfile = std::move(cb); }

    // Thread-safe: ask a running Start()/StreamFor() (e.g. on a worker thread)
    // to wind down early; it reacts within ~1 s and stops the scan as usual.
    void RequestStop() { m_abort.store(true); }

private:
    // RAII owner of a POSIX socket fd: closes on destruction / Reset(), so no
    // path can leak or double-close a descriptor. Movable, not copyable.
    class UniqueFd {
    public:
        UniqueFd() = default;
        explicit UniqueFd(int fd) noexcept : m_fd(fd) {}
        ~UniqueFd();
        UniqueFd(const UniqueFd&)            = delete;
        UniqueFd& operator=(const UniqueFd&) = delete;
        UniqueFd(UniqueFd&& o) noexcept : m_fd(o.m_fd) { o.m_fd = -1; }
        UniqueFd& operator=(UniqueFd&& o) noexcept;
        [[nodiscard]] int  Get()   const noexcept { return m_fd; }
        [[nodiscard]] bool Valid() const noexcept { return m_fd >= 0; }
        void Reset(int fd = -1) noexcept;    // close current, adopt fd
    private:
        int m_fd = -1;
    };

    // protocol constants (inline static constexpr)
    static constexpr int    PORT_COMMAND   = 6100;
    static constexpr int    PORT_STREAM    = 6105;
    static constexpr double RANGE_SCALE_M  = 0.0001;    // 1/10 mm raw range unit (SDK)
    static constexpr const char* RPS_FLAG  = "-speed";  // inferred firmware flags
    static constexpr const char* MHZ_FLAG  = "-datarate";
    enum { ZFS_HEADER_ID = 4, LINE_HEADER_ID = 3, RAW_PIXEL_ID = 6, COMP_PIXEL_ID = 7 };
    enum FrameRes { FRAME_OK, FRAME_TIMEOUT, FRAME_ERR };

    Config m_cfg;
    std::string m_local, m_scanCmd;
    UniqueFd m_cmdFd, m_streamFd;
    bool m_scanning = false;
    int  m_pixel = -1;
    long m_lines = 0, m_pts = 0;
    std::vector<unsigned char> m_payload, m_inflated, m_lineHeader;
    long m_dupLog = 0;            // rate-limit dropped-line log messages
    int  m_burstCool = 0;         // garbled exit lines after a no-return burst

    ProfileCallback   m_onProfile;
    std::atomic<bool> m_abort{false};

    std::string buildScanCommand() const;
    FrameRes    readFrame(uint16_t& type, std::vector<unsigned char>& payload);
    long        decodeProfile(const unsigned char* data, size_t n, double t_s, double x);

    // generic helpers (no scanner state). Sockets are O_NONBLOCK; every wait
    // goes through waitFd()/poll() with a timeout, so no call can hang.
    static bool        setNonBlocking(int fd);
    static bool        waitFd(int fd, short events, double timeout_s);
    static bool        recvExact(int fd, void* buf, size_t n, double idle_timeout_s);
    static bool        sendAll(int fd, const std::string& s, double timeout_s);
    static int         connectTcp(const std::string& ip, int port, double timeout_s = 5.0);
    static std::string localIp(const std::string& ip);
    static uint32_t    rdU32(const unsigned char* p);
    static int         parseHeaderPixel(const unsigned char* d, size_t n);
    static size_t      inflateLine(const unsigned char* in, size_t inlen,
                                   std::vector<unsigned char>& out);
    static bool        statusInt(const std::string& xml, long& out);
    static std::string affirmative(const std::string& xml);
    static std::string statusStr(long s);
};
