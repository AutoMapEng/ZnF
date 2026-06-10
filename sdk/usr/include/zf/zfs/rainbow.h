#pragma once

#include <functional>
#include <vector>

#include "zfsapi.h"
#include "zfrgb.h"
#include "hsv.h"


namespace zfs
{
    ZFS_API std::vector<zfs::math::rgb> CreateRainbow(int typ = 1);

    // return a color from a predefined table
    ZFS_API zfs::math::rgb autocolor(int i);

    //#############################################################################################################

    inline math::rgb basic_color_generator(int i, size_t entries)
    {
        const double h = (double)i / entries * 240.;
        auto rgb = zf::hsv2rgb({ h,1.,1. });
        return { static_cast<uint8_t>(rgb.r * 255),
                 static_cast<uint8_t>(rgb.g * 255),
                 static_cast<uint8_t>(rgb.b * 255) };
    }

    /**
    \brief create a color table with n entries
    */
    class ColorTable
    {
    public:        
        ColorTable() = default;
        ColorTable(size_t entries, 
            std::function< math::rgb(int index, size_t entries)> generator = basic_color_generator);

        /// entries f the table
        size_t size() const { return tab.size(); }

        /// get color from index
        zfs::math::rgb operator()(unsigned int idx) const;

    protected:
        std::vector<zfs::math::rgb> tab;
    };


    //=========== inline ================================================

    inline ColorTable::ColorTable(size_t entries,
        std::function< math::rgb(int index, size_t size)> generator)
    {
        if (entries)
        {
            tab.reserve(entries);
            for (size_t i = 0; i < entries; ++i)
                tab.push_back( generator((int)i, entries) );
        }
    }


    inline zfs::math::rgb ColorTable::operator()(unsigned int idx) const
    {
        if (size() == 0) return {};
        if (idx >= tab.size()) 
            return tab[tab.size() - 1]; 
        else 
            return tab[idx];
    }
}