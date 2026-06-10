#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>
#include <vector>

#include <utility>

#include <zfs/Math/zfvec.h>

#include <zfs/theoModel.h>
#include <zfs/noise.h>

#include <zfs/Math/fit/surfacefit_xyz.h>
#include <zfs/Math/object/table.h>

#include <zfs/Math/fit/fit_plane.h>
#include <zfs/Math/fit/fit_cylinder.h>

#include "plane_parameter_matrix.h"
#include "plane_data_point.h"
#include "plane_kdtree.h"
#include "plane_connector_parameter.h"
#include "plane_connect_thresholds.h"

namespace zfsfilter
{
    namespace planes
    {
        // ========================================================================================================================

        using vec = std::vector<double>;
        using vec_ui32 = std::vector<uint32_t>;
        using pt = std::pair<uint32_t, uint32_t>;
        using range_d = std::pair<double, double>;

        // ========================================================================================================================
        // class for storing multiple plane data
        // ========================================================================================================================

        class FILTERDLL_API collected_plane_data_t
        {
        public:

            struct select_option_parameter_t // this is for generating plane table
            {
                double   min_area{ 0. };
                double   max_area{ std::numeric_limits<double>::max() };
                uint32_t min_count{ 0 };
                uint32_t min_line_count{ 0 };
                uint32_t min_pixel_count{ 0 };

                bool operator==(const select_option_parameter_t& rhs) const
                {
                    return min_area == rhs.min_area && max_area == rhs.max_area && min_count == rhs.min_count;
                }

                bool pre_condition(const plane_data_point_ptr_t& plane_data_point_ptr) const
                {
                    return condition(plane_data_point_ptr->area, plane_data_point_ptr->cnt, plane_data_point_ptr->cnt_lines, plane_data_point_ptr->cnt_pixel);
                }

                bool condition(const double& area, const uint32_t& cnt, const uint32_t& cnt_lines, const uint32_t& cnt_pixel) const
                {
                    return area >= min_area && area <= max_area && cnt >= min_count && cnt_lines >= min_line_count && cnt_pixel >= min_pixel_count;
                }

                bool condition(const plane_data_point_ptr_t& plane_data_point_ptr) const
                {
                    return plane_data_point_ptr->valid &&
                        condition(plane_data_point_ptr->area,plane_data_point_ptr->cnt, plane_data_point_ptr->cnt_lines, plane_data_point_ptr->cnt_pixel);
                }

                select_option_parameter_t() = default;
                select_option_parameter_t( const plane_connect_thresholds_t& plane_connect_thresholds)
                    : min_area ( plane_connect_thresholds.min_area_connected_plane),
                      max_area(plane_connect_thresholds.max_area_connected_plane),
                      min_count(plane_connect_thresholds.min_cnt_connected_plane),
                      min_line_count(plane_connect_thresholds.min_line_cnt_connected_plane),
                      min_pixel_count(plane_connect_thresholds.min_pixel_cnt_connected_plane)
                {}
            };

            struct collect_data_switches_t // < used for adding subplane data in plane connector
            {
                bool collect_subplane_sample_pt{ true };
                bool collect_sample_pts{ true };
                bool collect_contour_pts{ false };
                bool collect_plane_edge_points{ false };
                bool element_near_horizon{ false };
                bool collect_extended_plane_data{ false };

                collect_data_switches_t() = default;
                collect_data_switches_t(const plane_connector_parameter_t& plane_connector_parameter)
                    : collect_contour_pts{ plane_connector_parameter.collect_data_for_contour_or_convex_hull },
                    collect_plane_edge_points{ plane_connector_parameter.collect_plane_edge_points }
                {}
            };

            // ================================================================================================================

            collected_plane_data_t() = default;

            collected_plane_data_t(const plane_finder_scan_section_processing_parameter_t& scan_section_processing_parameter, plane_parameter_matrix_ptr_t plane_parameter_matrix_);

            ~collected_plane_data_t() = default;

            // ================================================================================================================

            plane_data_point_ptr_t get_cur_data_point(const uint32_t& plane_idx);

            void set_base( plane_data_point_ptr_t cur_data_point,
                           const int32_t& plane_matrix_subplane_line_idx,
                           const int32_t& plane_matrix_subplane_pixel_idx,
                           const plane_parameter_ptr_t& plane_parameter);

            void set_plane_tangents( plane_data_point_ptr_t cur_data_point,
                                     const vec& plane_tangent_u,
                                     const vec& plane_tangent_v);

            void set_data_sample_pts(plane_data_point_ptr_t cur_data_point,
                const plane_parameter_ptr_t& plane_parameter,
                const collect_data_switches_t& collect_data_switches);

            void set_subplane_sample_pts( plane_data_point_ptr_t cur_data_point,
                                          const plane_parameter_ptr_t& plane_parameter,
                                          const collect_data_switches_t& collect_data_switches);

            void add(const uint32_t& plane_idx,
                const int32_t& plane_matrix_subplane_line_idx,
                const int32_t& plane_matrix_subplane_pixel_idx,
                const plane_parameter_ptr_t& plane_parameter,
                const collect_data_switches_t& collect_data_switches);

            bool set() const
            {
                return b_set;
            }

            void set(bool b_set_)
            {
                b_set = b_set_;
            }

            vec getAreas() const;

            vec_ui32 getCnts() const;

            std::vector<vec> getNormals();

            std::vector<vec> getNormalStds();

            std::vector<vec> getCenterPositions();

            std::vector<vec> getCenterPositionStds();

            zfs::math::convex_hull_ptr_vector_t<plane_contour_pt_ptr_t> getConvexHulls();

            uint32_t getNoOfPlanes() const
            {
                return max_idx;
            }

            void calculate_overall_plane_data();

            void calculate_overall_plane_data_accumulation();

            // ================================================================================================================

            enum class object_type_t : uint32_t {plane, cylinder, unknown};

            using object_fit_data_for_decision_t = std::pair< object_type_t, double >; // std::pair<fit type, standard deviation of fit>

            object_type_t goodness_of_fit_decision( const object_fit_data_for_decision_t& fit_decision_data_1, const object_fit_data_for_decision_t& fit_decision_data_2, const double& blind_section_factor);

            object_type_t check_on_object_type(const PlaneFit& plane_fit, const CylinderFit& cylinder_fit);

            // ================================================================================================================

            void calculate_overall_plane_data_point_cloud_fit();

            const plane_data_point_ptr_vector_t& get_plane_data_points() const;

            const plane_data_point_ptr_vector_t& get_selected_plane_data_points(const select_option_parameter_t& select_option_parameter);

            data_table<int>& get_table(const select_option_parameter_t& select_option_parameter);

            set_s get_current_plane_table_column_content_string_identifiers() const;

            const plane_kdtree_t& get_kdtree(const select_option_parameter_t& select_option_parameter);

            const plane_connector_parameter_t& get_plane_connecting_parameter() const
            {
                return plane_connector_parameter;
            }

            uint32_t get_number_of_cylinder_fits_conducted() const
            {
                return number_of_cylinder_fits_conducted;
            }

        private:
            // ================================================================================================================
            struct extract_item_generation_state_t
            {
                bool   generated{ false };

                collected_plane_data_t::select_option_parameter_t select_option_parameter;

                void clear()
                {
                    generated = false;
                    select_option_parameter = collected_plane_data_t::select_option_parameter_t();
                }

                void set(const collected_plane_data_t::select_option_parameter_t& select_option_parameter_)
                {
                    generated = true;
                    select_option_parameter = select_option_parameter_;
                }

                bool check(const collected_plane_data_t::select_option_parameter_t& select_option_parameter_) const
                {
                    return (select_option_parameter == select_option_parameter_);
                }
            };

            // ================================================================================================================
            bool                       b_set{ false };
            bool                       b_overall_plane_data_calculated{ false };

            uint32_t                   max_idx{ 0 };

            double                     last_table_min_area{ 0 };
            uint32_t                   last_table_min_count{ 0 };

            uint32_t                   number_of_cylinder_fits_conducted{ 0 };

            vec                        normal_sigma{ vec(3,0) }; // < intermediate storage of sigmas
            vec                        center_position_sigma{ vec(3,0) }; // < intermediate storage of sigmas

            noise_parameter_t          noise_parameter; // < range noise parameter for validation of colleced plane

            select_option_parameter_t       plane_selection; // < parameters for validating plane
            plane_connector_parameter_t     plane_connector_parameter; // < options of the plane connector

            plane_parameter_matrix_ptr_t   plane_parameter_matrix; // < ptr to plane parameter matrix

            plane_data_point_ptr_vector_t   plane_data_points; // < shared ptr's due to use of kd-tree

            extract_item_generation_state_t table_generation_state;
            data_table<int>                 plane_data_table;

            extract_item_generation_state_t kdtree_generation_state;
            plane_kdtree_t                  plane_data_kdtree;

            extract_item_generation_state_t selected_plane_data_points_generation_state;
            plane_data_point_ptr_vector_t   selected_plane_data_points;

            // ================================================================================================================

            void feed_data_point_vector(const select_option_parameter_t& select_option_parameter);
            void feed_table(const select_option_parameter_t& select_option_parameter);
            void feed_kdtree(const select_option_parameter_t& select_option_parameter);
        };
    }
}
