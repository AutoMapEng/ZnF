#pragma once

#include "zfsapi.h"
#include "../Common/inttypes_.h"

#include <memory>

class ZFSHeader;

namespace zfs
{
    /**
    * @brief fast access from pixel index to angle. Works just for ENC sync like typ 114 and 214
    */
    class ZFS_API VertPixel2Angle
    {
    public:
        VertPixel2Angle() : pixelPerLineZfs(0) {}
        virtual ~VertPixel2Angle() { }

        /// init
        bool createTable(const ZFSHeader& zhead);

        bool isValid() const;

        /// return angle of pixel [rad]
        double angle(uint32_t pixelIndexZfsRaw) const;
        double operator()(uint32_t pixelIndexZfsRaw) const;

    protected:
        std::shared_ptr<double> angle_enc_h{};    ///< the table. for every pixel there exist an angle [rad]
        uint32_t pixelPerLineZfs{};    ///< number of pixel in the table
    };


    //################################################################################################

    inline bool VertPixel2Angle::isValid() const
    {
        return pixelPerLineZfs ? true : false;
    }

    inline double VertPixel2Angle::angle(uint32_t pixelIndexZfsRaw) const
    {
        if (pixelIndexZfsRaw < pixelPerLineZfs)
            return angle_enc_h.get()[pixelIndexZfsRaw];
        return 0.;
    }

    inline double VertPixel2Angle::operator()(uint32_t pixelIndexZfsRaw) const
    {
        return angle(pixelIndexZfsRaw);
    }

}
