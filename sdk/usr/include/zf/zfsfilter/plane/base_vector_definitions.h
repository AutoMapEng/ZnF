#pragma once

#include <vector>
#include <utility>
#include <zfs/zfrgb.h>

namespace zfsfilter
{
    namespace planes
    {
        // ======================================================================================================================
        using tuple_i = std::pair<int, int>;
        using tuple_d = std::pair<double, double>;
        using range_d = std::pair<double, double>;
        using vec_b = std::vector<bool>;
        using vec_i = std::vector<int>;
        using vec = std::vector<double>;
        using vec_vec = std::vector<vec>;
        using vec_tuple = std::vector<tuple_d>;
        using vec_rgb = std::vector<zfs::math::rgb>;
        // ======================================================================================================================
    }
}
