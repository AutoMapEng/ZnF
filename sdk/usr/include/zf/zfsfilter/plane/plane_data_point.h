#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>

#include <zfs/Math/stl/vector_op.h>
#include <zfs/Math/math/minmax.h>
#include <zfs/Math/statistics/mean_3d.h>
#include <zfs/Math/2d/contour.h>
#include <zfs/Math/3d/plane_convex_hull.h>
#include <zfs/Math/3d/plane_center_distance_calculator.h>

#include <zfsfilter/plane/plane_connector_parameter.h>
#include <zfsfilter/plane/plane_point_types.h>
#include <zfsfilter/plane/plane_parameter_matrix.h>

namespace zfsfilter
{
    namespace planes
    {
        // ========================================================================================================================
        using vec = std::vector<double>;
        // ========================================================================================================================
        // class holding plane data during and after plane connecting
        // ========================================================================================================================
        struct FILTERDLL_API plane_data_point_t
        {
            bool valid{ false };

            int plane_idx{ -1 };

            uint32_t cnt{ 0 };
            double   area{ 0 };

            uint32_t cnt_lines{ 0 };
            uint32_t cnt_pixel{ 0 };

            zfs::math::mean plane_center_azimuth;
            zfs::math::mean plane_center_elevation;

            // zfs::math::Average plane_center_line;
            // zfs::math::Average plane_center_pixel;

            zfs::math::MinMax<int32_t> plane_matrix_subplane_line;
            zfs::math::MinMax<int32_t> plane_matrix_subplane_pixel;

            // -- plane parameter --
            double range_to_center_position{ 0 };
            zfs::math::planes::plane_distance_calculator plane_distance;

            plane_connector_parameter_t::collected_plane_parameter_retrieval_type_t 
                collected_plane_parameter_retrieval_type{ plane_connector_parameter_t::collected_plane_parameter_retrieval_type_t::fit }; // which type of plane parameter retrieval is used

            // -- variant A -- use accumulation to get collect plane parameters --
            zfs::math::average_3D   normal;
            zfs::math::mean_3D      normal_error;

            zfs::math::average_3D   center_position;
            zfs::math::mean_3D      center_position_error;

            zfs::math::mean_3D      plane_tangent_u;
            zfs::math::mean_3D      plane_tangent_v;

            // -- variant B -- use point cloud and fit or use plane parameter matrix, subsampling, and fit -- fit_plane is used
            vec    plane_normal_from_fit{ vec(3,0) };
            vec    plane_center_from_fit{ vec(3,0) };
            vec    plane_tangent_1_from_fit{ vec(3,0) };
            vec    plane_tangent_2_from_fit{ vec(3,0) };
            double standard_deviation_of_fit{ 0 };

            uint32_t elements_near_horizont_cnt{ 0 };

            plane_sample_pt_ptr_vector_t plane_sample_pt_near_horizon_ptr_vector;
            plane_sample_pt_ptr_vector_t plane_sample_pt_ptr_vector;

            plane_contour_pt_ptr_vector_t plane_contour_pt_ptr_vector;

            plane_pt_with_error_information_ptr_vector_t plane_pt_with_error_ptr_vector;

            zfs::math::plane_convex_hull<plane_contour_pt_t> plane_convexhull;

            plane_data_point_t();
            plane_data_point_t(const uint32_t& plane_idx_);

            double get_perpendicular_plane_distance_to_0() const;
            const vec& get_normal() const;
            const vec& get_center_position() const;
            const double& get_normal(uint32_t idx) const;
            const double& get_center_position(uint32_t idx) const;

            std::pair<vec, vec> get_plane_tangents() const;

            plane_parameter_ptr_t get_plane_parameter_ptr();

        };
        // ========================================================================================================================
        using plane_data_point_ptr_t= std::shared_ptr<plane_data_point_t>;
        // ========================================================================================================================
        typedef std::vector<plane_data_point_t> plane_data_point_vector_t;
        typedef std::vector<plane_data_point_ptr_t> plane_data_point_ptr_vector_t;
        // ========================================================================================================================
    }
}