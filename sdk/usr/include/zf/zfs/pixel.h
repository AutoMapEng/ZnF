#pragma once

#include <cstdint>
#include <vector>
#include <deque>
#include <utility>

#include "zfsapi.h"

#include "Math/zfvec.h"
#include "Math/rgb.h"

namespace zfs
{
    /**
    \brief  the pixel information from the scan file
            include only
    */
    class ZFS_API PDataRaw
    {
    public:

        uint32_t rf{ 0 };   ///< intensity [increments]
        double rg{ 0 };		///< measurement range [meter]
        float rfNorm{ 0 };	///< intensity normalized 0..1 depending on range and calibration 0=black surface 1=white surface >1=overloaded
        uint8_t add{0 }; ///< additional byte if available
        uint8_t mask{ 0 };  ///< pixel not valid if mask bit is set.
        uint8_t filterID{0}; ///< typ of filter used for filter out point

        enum mask_t
        {
            MASK_FILTERED_OUT = 1,  ///< pixel was filtered out
            MASK_MODIFIED = 2,      ///< pixel was modified
            _MASK_ = 4,
        };

        /// set new intensity
        void setRf(uint32_t newrf) { if (rf != newrf) { rf = newrf;  mask |= MASK_MODIFIED; } }

        /// set new range [meter]
        void setRg(uint32_t newrg) { if (rg != newrg) { rg = newrg;  mask |= MASK_MODIFIED; } }

        /// check wheter mask is set or not
        bool isMask() const { return mask ? true : false; }

        // rfNorm1 clipped to max 1.
        float RfNorm1() const { return rfNorm >= 1.f ? 1.f : rfNorm; }
    };

    /// additional calculated values like 3d point
    class ZFS_API PData : public PDataRaw
    {
    public:
        zfs::math::Vec3d xyz;	///< local 3D [meter]

        zfs::math::Vec3d getXYZ() const { return xyz; }

        // get
        const double& x() const { return xyz.x(); }
        const double& y() const { return xyz.y(); }
        const double& z() const { return xyz.z(); }

        // set
        double& x() { return xyz.x(); }
        double& y() { return xyz.y(); }
        double& z() { return xyz.z(); }
    };

    /// another additional value RGB
    class ZFS_API PDataRGB : public PData
    {
    public:
        zfs::math::rgb rgb;		///< color red,green,blue each 0..255

        /// color information
        zfs::math::rgb getRGB() const { return rgb; }
    };

    // forth dimension (time)
    class ZFS_API PData4D : public PDataRGB
    {
    public:
        double timeSec {};

        /// the time as seconds
        double time() const { return timeSec; }
    };

    class ZFS_API PDataDetail : public PData4D
    {
    public:
        uint32_t px{};  ///< the pixel index of the orginal zfs file
        uint32_t ln{};  ///< typically the line is set in the PDatas but for unordered points like from slam, the information has to be set for each pixel. Not used for imager at the moment
        std::pair<double, double> angle;    ///< first=vertical  second=horizontal

        double getVerticalRad() const { return angle.first; }
        double getHorizontalRad() const { return angle.second; }
    };

    class ZFS_API PDataNormal : public PDataDetail
    {
    public:
        zfs::math::Vec3f normal;	///< has a normal vector as well (just float)

    };

    //=========================================================================================================================

    template<class T>
    class PDatas
    {
    public:
        bool valid { false }; ///< true if there are datas available
        uint32_t line {}; ///< line number from zfs-file
        uint32_t part {}; ///< optional the part index
        int32_t minrf {}; ///< minimum intensity [inc]
        double refRg {};   ///< reference range [meter]
        double rgoffset {};
        int32_t refRf {};      ///< reference intensity [inc]
        uint32_t lhFlags {}; ///< the flags field from the lineheader required by the filter
        uint8_t counterByte {}; ///< zero if no counterbyte, otherwise the index
        uint32_t mask {};    ///< additional info about the line

        enum
        {
            LMASK_BADLINE = 1,   ///< this line is invalid and has to be filtered out
            LMASK_CRITICAL_COMPENSATOR = 2, ///< this line has to be filtered out because too much movement detected
        };

        uint64_t available{};
        enum
        {
            A_REFERENCE = (1<<0), ///< reference intensity and range
            A_RFNORM = (1<<1),   ///< rfnorm possible and available
        };

        std::vector<T> points;

        PDatas() = default;
        PDatas(int pts) { points.resize(pts); }

        auto size() const { return points.size(); }

        void clr()
        {
            line = 0;
            minrf = 0;
            refRg = 0;
            rgoffset = 0;
            refRf = 0;
            lhFlags = 0;
            counterByte = 0;
            mask = 0;
        }
    };

    template class ZFS_API PDatas<PDataRaw>;
    template class ZFS_API PDatas<PData>;
    template class ZFS_API PDatas<PDataRGB>;
    template class ZFS_API PDatas<PDataDetail>;

    //=========================================================================================================================

}
