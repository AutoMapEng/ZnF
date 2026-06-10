#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>

#include <zfsfilter/tools/scan_parameter.h>

namespace zfsfilter
{
    namespace planes
    {
        // ========================================================================================================================

        // set during initialization of plane_finder_processing_result data structure from the content of the planefinder_parameter

        struct plane_connector_parameter_t
        {
            // -- definitions -----------------------------------------------

            enum class plane_collection_comparison_operation_type_t : uint32_t { uninitialized, subplane2startplane, subplane2subplane, subplane2collectedplane };
            enum class collected_plane_parameter_retrieval_type_t : uint32_t { uninitialized, averaged, fit };

            // --------------------------------------------------------------

            bool collect_extended_plane_data{ false }; // < collect not just basic data
            bool collect_data_for_contour_or_convex_hull{ false }; // < collect data for contour or convex hull
            bool collect_plane_edge_points{ false }; // < when contours or convex hulls are calculated, collect not just the center points of the planes, but the edge points (gives more exact enclosing curves and areas)
            bool calculate_convex_hull{ false }; // < calculate convex hull
            bool calculate_plane_area_from_convex_hull{ false };// < calculate plane area from convex hull
            bool calculate_plane_distance{ true }; // < calculate plane distance (d in: _n_*_p_-d = 0)
            bool calculate_precision_plane{ true }; // < a fit is conducted that discards (a limited number of) outliers, is set from planefinder_parameter.operation_settings
            bool harmonize_normals{ true }; // < set from planefinder_parameter, normals are all looking away/towards co-ordinates center, otherwise normals are left unchanged (see also switch: normals_looking_away_from_coordinate_center)
            bool generate_normals_looking_away_from_coordinate_center{ false }; // < set from planefinder_parameter, setting of the normal vector direction: true: away, false: towards co-ordinates center
            bool use_plane_center_sample_pts_for_plane_fits{ false }; // < "default" is to use the positions for (u,v) = (0,0) of the parametric (sub)plane fits of the first stage of the plane finder

            // plane parameter retrieval options
            collected_plane_parameter_retrieval_type_t plane_parameter_default_retrieval_type{ collected_plane_parameter_retrieval_type_t::uninitialized };
            collected_plane_parameter_retrieval_type_t plane_parameter_retrieval_type{ plane_parameter_default_retrieval_type };

            plane_collection_comparison_operation_type_t plane_collection_comparison_default_operation_type{ plane_collection_comparison_operation_type_t::uninitialized };
            plane_collection_comparison_operation_type_t plane_collection_comparison_operation_type{ plane_collection_comparison_default_operation_type };

            // processing
            int n_threads{ 1 }; // number of threads used, when option collected_plane_parameter_retrieval_type_t::fit is choosen , is set from planefinder_parameter.operation_settings

            // plane connector algorithm test mode
            bool test_mode{ false };

            plane_connector_parameter_t() = default;

            plane_connector_parameter_t(bool collect_extended_plane_data_,
                bool collect_data_for_contour_or_convex_hull_,
                bool calculate_convex_hull_,
                bool calculate_plane_area_from_convex_hull_,
                bool calculate_plane_distance_,
                bool calculate_precision_plane_,
                collected_plane_parameter_retrieval_type_t plane_parameter_retrieval_type_,
                plane_collection_comparison_operation_type_t plane_collection_comparison_operation_type_,
                int n_threads_,
                bool harmonize_normals_,
                bool generate_normals_looking_away_from_coordinate_center_,
                bool collect_plane_edge_points_,
                bool use_plane_center_sample_pts_for_plane_fits_,
                bool test_mode_switch_ = false)
                : collect_extended_plane_data(collect_extended_plane_data_),
                  collect_data_for_contour_or_convex_hull(collect_data_for_contour_or_convex_hull_),
                  collect_plane_edge_points(collect_plane_edge_points_),
                  calculate_convex_hull(calculate_convex_hull_ || calculate_plane_area_from_convex_hull_),
                  calculate_plane_area_from_convex_hull(calculate_plane_area_from_convex_hull_),
                  calculate_plane_distance(calculate_plane_distance_),
                  calculate_precision_plane(calculate_precision_plane_),
                  use_plane_center_sample_pts_for_plane_fits(use_plane_center_sample_pts_for_plane_fits_),
                  plane_parameter_retrieval_type(plane_parameter_retrieval_type_),
                  plane_collection_comparison_operation_type(plane_collection_comparison_operation_type_),
                  n_threads(n_threads_),
                  test_mode(test_mode_switch_)
            {}
        };
        
        // ========================================================================================================================
    }
}

