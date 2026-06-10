#pragma once
#include "zfcore_api.h"
#include "compiler.h"

#include <string>


namespace zf
{
    /**
    @brief Version string mnagement
    */
    class ZFCORE_API Version
    {
    public:
        /// version string int the format like "1.0.0"
        Version() = default;
        Version(const std::string& s) : m_sver(s) { split(); }
        Version(int mj, int mi = 0, int pa = 0) : m_majorVer(mj), m_minorVer(mi), m_patchVer(pa) {}

        /// set to zero
        void clear() { m_majorVer = m_minorVer = m_patchVer = 0; m_sver.clear(); }

        /// rarse a string and fillup xxxVer
        void parse(const std::string& s) { m_sver = s;  split(); }

        bool isZero() const;

        /// test if at least defined version
        bool isMinVer(int testmaj, int testmin, int testpatch = 0) const;
        bool isVer(int testmaj, int testmin, int testpatch = 0) const { return isMinVer(testmaj, testmin, testpatch); } // for old code

        /// test for exact version
        bool isVerExact(int testmaj, int testmin, int testpatch = 0) const;

        // getter
        int majorVer() const { return m_majorVer; }
        int minorVer() const { return m_minorVer; }
        int patchVer() const { return m_patchVer; }

        /// set minor and below to zero
        void zeroMinor() { m_minorVer = 0; m_patchVer = 0; }
        /// set patch to zero
        void zeroPatch() { m_patchVer = 0; }

    protected:
        std::string m_sver;   ///< the orginal string
        int m_majorVer{};
        int m_minorVer{};
        int m_patchVer{};

        /// split the string to ints
        void split();
    };


} // end namespace zfs


