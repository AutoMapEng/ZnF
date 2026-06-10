#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>
#include <vector>

#include <zfs/layer.h>

#include <zfsfilter/Matrix.h>

#include "plane_finder_collected_plane_data.h"
#include "plane_parameter_matrix.h"


namespace zfsfilter
{
    namespace planes
    {
        // ========================================================================================================================

        using vec = std::vector<double>;

        // ========================================================================================================================
        
        // ========================================================================================================================
        // class for plane connection
        // ========================================================================================================================
        class plane_connector
        {
            // ========================================================================================================================
            // class test_mode_meta_data
            // ========================================================================================================================
            struct test_mode_meta_data
            {
                
                bool test_mode_switched_on{ false };
                int32_t region_grow_test_start_line{ 0 };
                int32_t region_grow_test_start_pixel{ 0 };
                int32_t region_grow_plane_id_at_line_0_pixel_0{ 0 };

                test_mode_meta_data(bool test_mode_switched_on_);

                std::wstring get_test_mode_name_add();
                std::wstring get_data_set_name(const std::wstring& datasetname_);
                std::wstring get_test_output_directory_name(const std::wstring& datasetname_, const uint32_t& scan_section_idx);
                std::wstring get_test_file_name_prefix(const std::wstring& datasetname_, const uint32_t& scan_section_idx);
                std::wstring get_test_plane_parameter_file_name(const std::wstring& datasetname_, const uint32_t& scan_section_idx);
                std::wstring get_test_plane_convex_hull_file_name(const std::wstring& datasetname_, const uint32_t& scan_section_idx);
                std::wstring get_test_plane_id_matrix_file_name(const std::wstring& datasetname_, const uint32_t& scan_section_idx);
            };
            // ========================================================================================================================
            // class condition_checker
            // ========================================================================================================================
            struct condition_checker
            {
                plane_connect_thresholds_t plane_connect_thresholds;

                bool condition{ false }; // < true if condition is met
                double spv{ 0 }; // < contains squared_projection_value
                double ev{ 0 }; // < contains expected_error_variance

                condition_checker() = default;

                void initialize(const plane_connect_thresholds_t& plane_connect_thresholds_);

                // ===================================================================================================================================================
#if 0
                bool plane_connect_condition_0(const plane_parameter_ptr_t check_plane, const plane_data_point_ptr_t& collected_plane);
#endif
                // ===================================================================================================================================================

                bool plane_connect_condition_1(
                    const vec& normal_error_plane_1_,
                    const vec& normal_error_plane_2_,
                    const vec& center_difference_vector_error_plane_1_,
                    const vec& center_difference_vector_error_plane_2_);

                bool plane_connect_condition_1_partial(const double& error_component_1_, const double& error_component_2_);

                // incidence angle depend (problem of seperation of electronic noise from structure noise (with cosine dependence) in particular in case of normal error comparison)
                // alternative : compare to accumulated plane

                bool plane_connect_condition_1(
                    const vec& normal_error_plane_1_,
                    const vec& normal_error_plane_2_,
                    const vec& normal_error_electronic_plane_1_,
                    const vec& normal_error_electronic_plane_2_,
                    const vec& center_difference_vector_error_plane_1_,
                    const vec& center_difference_vector_error_plane_2_,
                    const vec& position_error_electronic_1_,
                    const vec& position_error_electronic_2_,
                    const double& cos_incidence_angle_1_,
                    const double& cos_incidence_angle_2_);

                // ===================================================================================================================================================

                bool plane_connect_condition_2( const vec& normal_,
                                                const vec& normal_error_,
                                                const vec& center_difference_vector_,
                                                const vec& center_difference_vector_error_ );

                double squared_projection_value(const vec& normal_, const vec& center_difference_vector_);

                double expected_error_variance( const vec& normal_,
                                                const vec& normal_error_,
                                                const vec& center_difference_vector_,
                                                const vec& center_difference_vector_error_);

                // ===================================================================================================================================================

            } ;

            condition_checker condition_check;

        public:

            plane_connector() = delete;
            plane_connector( const uint32_t& scan_section_idx,
                             plane_parameter_matrix_t& plane_matrix_,
                             collected_plane_data_t& plane_data_,
                             bool test_mode_switch_ = false);

            bool sub_plane_connect_condition_v1(const plane_parameter_ptr_t ref_plane, const plane_parameter_ptr_t check_plane, const plane_connect_thresholds_t& thresholds, const plane_connector_parameter_t::plane_collection_comparison_operation_type_t& plane_collection_comparison_operation_type);
            bool sub_plane_connect_condition_v2(const plane_parameter_ptr_t ref_plane, const plane_parameter_ptr_t check_plane, plane_data_point_ptr_t cur_collected_plane_data, const plane_connect_thresholds_t& thresholds, const plane_connector_parameter_t::plane_collection_comparison_operation_type_t& plane_collection_comparison_operation_type);

            void plane_flood_fill(const uint32_t& scan_section_idx, plane_parameter_matrix_t& plane_matrix, int32_t m, int32_t n, uint32_t current_plane_id);
            void plane_labelling(const uint32_t& scan_section_idx, plane_parameter_matrix_t& plane_matrix);

            const Matrix<int32_t>& get_connect_planes_image();

            bool create_directory_if_not_exists(const std::wstring& directory_);

            static std::wstring get_data_set_name(const std::wstring& datasetname_);
            static std::wstring get_output_directory_name(const std::wstring& datasetname_, const uint32_t& scan_section_idx);
            static std::wstring get_file_name_prefix(const std::wstring& datasetname_, const uint32_t& scan_section_idx);

            uint32_t get_number_of_cylinder_fits() const;

            bool write_connected_plane_matrix_layer(const std::wstring& datasetname_, const uint32_t& scan_section_idx= 0);
            bool write_connected_plane_meta_data_files(const std::wstring& datasetname_, const uint32_t& scan_section_idx, const plane_connect_thresholds_t& thresholds);
            bool write_convex_hull_file(const std::wstring& filename, const data_table<int>::key_t_vector& sort_vector);
            
            void set_plane_area_threshold(const double& thresh);

            collected_plane_data_t& get_collected_plane_data();
            data_table<int>& get_plane_data_table();

        private:

            plane_connector_parameter_t plane_connector_parameter; // < options for the plane connector
            test_mode_meta_data test_mode_configuration; // < contains switch and further configuration information

            double connected_plane_area_threshold{ 0 };

            vec center_difference_vector{ vec(3,0) };
            vec center_difference_vector_error{ vec(3,0) };

            plane_parameter_matrix_t& plane_matrix;
            collected_plane_data_t& plane_data;
            Matrix<int32_t> connected_planes;

            ZFLayerI32 ConnectedPlanes;


        };

        // ========================================================================================================================
    }
}
