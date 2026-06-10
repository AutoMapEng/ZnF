// acquire.cpp - CLI entry point for the Z+F PROFILER 9020 driver.
// All scanner-control logic lives in the ZnF9020 class (ZnF9020.h / ZnF9020.cpp);
// main() just parses options, constructs one, and calls start().
#include "ZnF9020.h"

#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <string>

static void usage() {
    fprintf(stderr,
        "usage: acquire <scanner-ip> [options]\n"
        "  --seconds N        acquire duration (default 30)\n"
        "  --mode tcpzip|tcp  line stream encoding (default tcpzip)\n"
        "  --resolution R     scan resolution (default 10000)\n"
        "  --quality Q        scan quality (default 50)\n"
        "  --rps V            laser motor speed [rev/s]  -> -speed V\n"
        "  --mhz V            laser data rate  [MHz]     -> -datarate V\n"
        "  --min-amplitude A  drop returns below A (default 2000)\n"
        "  --rotate-deg D     angle-zero/mount correction (default 90.8, calibrated)\n"
        "  --min-range M      drop returns closer than M metres (default 0 = keep all)\n"
        "  --no-unwrap        disable gated >6.55 m unwrapping (cap at 6.55 m)\n"
        "  --mixpix-deg D     mixed-pixel edge-trail filter angle (default 8, 0=off)\n"
        "  --print-every N    print every Nth pixel per profile (default 2048)\n"
        "  --spacing M        synthetic travel per profile for X (default 0 = stationary)\n"
        "  --file NAME        scan file name (default unique live_<timestamp>)\n"
        "  --extra \"...\"      extra raw flags appended to the scan command\n"
        "  --scan-args \"...\"  replace the whole scan command's arguments\n"
        "  --local-ip IP      override local IP advertised to scanner\n");
}

static bool parse_args(int argc, char** argv, ZnF9020::Config& a) {
    for (int i = 1; i < argc; ++i) {
        std::string k = argv[i];
        auto val = [&](const char* name) -> const char* {
            if (i + 1 >= argc) { fprintf(stderr, "missing value for %s\n", name); exit(2); }
            return argv[++i];
        };
        if (k == "-h" || k == "--help") { usage(); exit(0); }
        else if (k == "--seconds")        a.seconds    = atof(val("--seconds"));
        else if (k == "--mode")           a.mode       = val("--mode");
        else if (k == "--resolution")     a.resolution = val("--resolution");
        else if (k == "--quality")        a.quality    = val("--quality");
        else if (k == "--rps")          { a.rps = atof(val("--rps")); a.has_rps = true; }
        else if (k == "--mhz")          { a.mhz = atof(val("--mhz")); a.has_mhz = true; }
        else if (k == "--min-amplitude")  a.min_amp    = atof(val("--min-amplitude"));
        else if (k == "--rotate-deg")     a.rotate_deg = atof(val("--rotate-deg"));
        else if (k == "--min-range")      a.min_range_m = atof(val("--min-range"));
        else if (k == "--no-unwrap")      a.unwrap = false;
        else if (k == "--mixpix-deg")     a.mixpix_deg = atof(val("--mixpix-deg"));
        else if (k == "--print-every")    a.print_every = atoi(val("--print-every"));
        else if (k == "--spacing")        a.spacing    = atof(val("--spacing"));
        else if (k == "--file")           a.file       = val("--file");
        else if (k == "--extra")          a.extra      = val("--extra");
        else if (k == "--scan-args")      a.scan_args  = val("--scan-args");
        else if (k == "--local-ip")       a.local_ip   = val("--local-ip");
        else if (!k.empty() && k[0] == '-') { fprintf(stderr, "unknown option %s\n", k.c_str()); return false; }
        else if (a.ip.empty())            a.ip = k;
        else { fprintf(stderr, "unexpected arg %s\n", k.c_str()); return false; }
    }
    return !a.ip.empty();
}

int main(int argc, char** argv) {
    signal(SIGPIPE, SIG_IGN);
    ZnF9020::Config cfg;
    if (!parse_args(argc, argv, cfg)) { usage(); return 2; }

    ZnF9020 scanner(cfg);              // create the scanner controller
    return scanner.start() ? 0 : 1;    // start the system
}
