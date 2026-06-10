#pragma once

#include <zfcore/version.h>

namespace zfs
{
    namespace tools
    {
        class Version : public zf::Version 
        {
        };
    }
}

#if 0
#include "../zfsapi.h"

#include <string>


namespace zfs
{
    namespace tools
    {
        /**
        @brief Version string mnagement
        */
        class ZFS_API Version
        {
        public:
            /// version string int the format like "1.0.0"
            Version() = default;
            Version(const std::string& s) : sver(s) { split(); }
            Version(int mj, int mi=0, int pa=0) : majorVer(mj), minorVer(mi), patchVer(pa) {}

            /// set to zero
            void clear() { majorVer = minorVer = patchVer = 0; sver.clear(); }

            /// rarse a string and fillup xxxVer
            void parse(const std::string& s) { sver = s;  split(); }

            /// test if at least defined version
            bool isMinVer(int testmaj, int testmin, int testpatch = 0) const;
            bool isVer(int testmaj, int testmin, int testpatch = 0) const { return isMinVer(testmaj, testmin, testpatch); } // for old code

            /// test for exact version
            bool isVerExact(int testmaj, int testmin, int testpatch = 0) const;

            // getter
            int major() const { return majorVer; }
            int minor() const { return minorVer; }
            int patch() const { return patchVer; }

            /// set minor and below to zero
            void zeroMinor() { minorVer = 0; patchVer = 0; }
            /// set patch to zero
            void zeroPatch() { patchVer = 0; }

        protected:
            std::string sver;   ///< the orginal string
            int majorVer{};
            int minorVer{};
            int patchVer{};

            /// split the string to ints
            void split();
        };

    }

} // end namespace zfs

#endif
