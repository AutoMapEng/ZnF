#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>
#include <vector>
#include <memory>

#include <zfs/Math/object/table.h>

#include <zfsfilter/Matrix.h>

#include <zfsfilter/tools/time_measurement.h>

#include "plane_parameter_matrix.h"
#include "plane_finder_parameter.h"
#include "plane_finder_collected_plane_data.h"
#include "plane_finder_scan_section_processing_parameter.h"

namespace zfsfilter
{
    namespace planes
    {
        // ==============================================================================================

        using namespace zfs::math;
        using namespace zfsfilter;

        // ==============================================================================================

        using matrix_uint32_ptr_t = shared_ptr<Matrix<uint32_t>>;

        // ==============================================================================================

        using collected_planes_sections_data_t      = std::vector<shared_ptr<collected_plane_data_t>>;
        using plane_parameter_matrix_sections_data_t= std::vector<shared_ptr<plane_parameter_matrix_t>>;
        using time_measurement_sections_data_t      = std::vector<shared_ptr<algorithm_step_time_measurement_t>>;

        // ==============================================================================================
        struct plane_finder_processing_result_t
        {
            scan_section_preparation_result_t scan_section_preparation_result{ scan_section_preparation_result_t::no_error };

            collected_plane_data_t            collected_plane_data;
            plane_parameter_matrix_ptr_t      plane_parameter_matrix_ptr;
            algorithm_step_time_measurement_t algorithm_step_time_measurement;

            planefinder_parameter_t planefinder_parameter;

            plane_finder_processing_result_t(){}

            ~plane_finder_processing_result_t(){}

            plane_finder_processing_result_t(const plane_finder_scan_section_processing_parameter_t& scan_section_processing_parameter)
            {
                set(scan_section_processing_parameter);
            }

            void set(const plane_finder_scan_section_processing_parameter_t& scan_section_processing_parameter)
            {
                planefinder_parameter = scan_section_processing_parameter.planefinder_parameter;
                plane_parameter_matrix_ptr.reset( new plane_parameter_matrix_t(scan_section_processing_parameter) );
                collected_plane_data = collected_plane_data_t(scan_section_processing_parameter, plane_parameter_matrix_ptr);
                algorithm_step_time_measurement.set({ "find_planes_init", "not_enough_points_planes", "thread_initialization", "data_read", "find_planes_subroutine_execution", "relative_angles", "noise_data", "fit", "outlier_detection", "outlier_check", "refit", "plane_validation", "plane_parameter_setting", "mask_writing" });
            }

            bool scan_section_preparation_succeeded() const
            {
                return scan_section_preparation_result == scan_section_preparation_result_t::no_error;
            }

            data_table<int>::sorted_table_data_extract_t get_sorted_plane_data_table(set_s& column_content_string_identifiers_)
            {
                data_table<int>& plane_data_table =
                    collected_plane_data.get_table(collected_plane_data_t::select_option_parameter_t(planefinder_parameter.plane_connect_thresholds));

                column_content_string_identifiers_ =
                    collected_plane_data.get_current_plane_table_column_content_string_identifiers();

                data_table<int>::sorted_table_data_extract_t extract = plane_data_table.get_sorted_table_extract("area", "descend");

                //log += boost::str(boost::format("-- plane data table generation took: %1% seconds\n") % time_take.duration2last());

                return extract;
            }

            const plane_kdtree_t& get_kdtree()
            {
                const plane_kdtree_t& section_planes_data_kdtree =
                    collected_plane_data.get_kdtree(collected_plane_data_t::select_option_parameter_t(planefinder_parameter.plane_connect_thresholds));

                return section_planes_data_kdtree;
            }

            plane_data_point_ptr_vector_t get_selected_plane_data_points()
            {
                return collected_plane_data.get_selected_plane_data_points({});
            }

            matrix_uint32_ptr_t get_plane_idx_matrix() const
            {

                plane_parameter_matrix_t plane_matrix = *plane_parameter_matrix_ptr;

                matrix_uint32_ptr_t PlaneIdxMatrixPtr;

                const int32_t lines  = plane_matrix.lines();
                const int32_t pixels = plane_matrix.pixels();

                PlaneIdxMatrixPtr= make_shared < Matrix<uint32_t> >(lines,pixels);

                Matrix<uint32_t>& PlaneIdxMatrix = *PlaneIdxMatrixPtr;

                for (int32_t cur_line = 0; cur_line < lines; ++cur_line)
                {
                    for (int32_t cur_pixel = 0; cur_pixel < pixels; ++cur_pixel)
                    {
                        PlaneIdxMatrix[cur_line][cur_pixel] = plane_matrix(cur_line, cur_pixel)->get_plane_id();
                    }
                }

                return PlaneIdxMatrixPtr;
            }

        };
        // ==============================================================================================
        typedef std::shared_ptr<plane_finder_processing_result_t> plane_finder_processing_result_ptr_t;
        // ==============================================================================================
    }
}
