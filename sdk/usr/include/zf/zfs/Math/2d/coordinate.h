#pragma once

#include <zfs/zfsapi.h>

#include <cstdint>
#include <utility>

namespace zfs { 
    namespace math {
    // =====================================================================================================================================
    struct ZFS_API coordinate_t
    {
        using pt_t = std::pair<int64_t, int64_t>;

        pt_t pt_data;

        coordinate_t() {}
        coordinate_t(const int64_t& u, const int64_t& v):pt_data(pt_t(u,v))
        {}

        void set(const int64_t& u, const int64_t& v)
        {
            pt_data.first = u;
            pt_data.second = v;
        }

        coordinate_t& operator+=(const coordinate_t& rhs)
        {
            pt_data.first += rhs.pt_data.first;
            pt_data.second += rhs.pt_data.second;

            return *this;
        }

        bool operator==(const coordinate_t& rhs) const
        {
            return (pt_data.first == rhs.pt_data.first && pt_data.second == rhs.pt_data.second);
        }

        const int64_t& u() const
        {
            return pt_data.first;
        }

        int64_t& u()
        {
            return pt_data.first;
        }


        const int64_t& v() const
        {
            return pt_data.second;
        }

        int64_t& v()
        {
            return pt_data.second;
        }

    };
    // =====================================================================================================================================
    coordinate_t ZFS_API operator+(const coordinate_t& lhs, const coordinate_t& rhs);
    // =====================================================================================================================================

    } // end namespace math
} // end namespace zfs