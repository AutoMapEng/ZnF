#pragma once

#include <zfs/zfsapi.h>

#include <cstdint>
#include <vector>

namespace zfs {
    namespace math {
        // ==============================================================
        using vec_b = std::vector<bool>;
        using vec_i = std::vector<int>;
        using vec = std::vector<double>;
        using tuple_i = std::pair<int, int>;
        using vec_3d_t = std::vector<double>;
        // ==============================================================
        struct ZFS_API plane_t
        {
            // base/const
            vec_3d_t zero_vec_3d{ vec(3, 0) }; //zero_vec= {0,0,0};

            // member
            vec_3d_t normal{ zero_vec_3d };// < normal 
            vec p0{ zero_vec_3d }; // < plane offset point
            double d0{ 0 };

            // ----------------------
            plane_t();
            plane_t(const vec_3d_t& normal_);
            plane_t(const vec_3d_t& normal_, const vec& p0_);

            void set(const vec_3d_t& normal_, const vec& p0_);

            void set_default(); // x-y-plane, <t1,t2,normal> form the unit vectors of the rect-linear co-ordinate system <e_x,e_y,e_z>

            vec_3d_t get_normal() const;
            vec get_p0() const;

            // ----------------------

            vec_3d_t get_t1() const;
            vec_3d_t get_t2() const;

            vec project_point_onto_plane(const vec& pt) const;

            // ----------------------

            double z_from_xy(const double& x, const double& y) const;

        private:
            // member
            vec_3d_t t1{ zero_vec_3d }, t2{ zero_vec_3d }; // < plane tangents

            // ----------------------
            vec cross(const vec& a, const vec& b) const;
            bool calculate_plane_tangents();

        };
        // ==============================================================

    } // end namespace math
} // end namespace zfs