#pragma once
#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include "../../pixel.h"

namespace zfs
{



    template<typename T>
    inline void disable(T& pd, int id)
    {
        pd.filterID = id;
    }

    template<typename T>
    inline void disable_nooverride(T& pd, int id)
    {
        if( pd.filterID==0 )
            pd.filterID = id;
    }

    template<typename T>
    inline void disableLine(PDatas<T>& l, int id)
    {
        for (auto& i : l.points)
            i.filterID = id;
    }



    //####################################################################

} // end namespace
