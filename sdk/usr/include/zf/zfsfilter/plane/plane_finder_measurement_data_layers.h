#pragma once

#include <zfsfilter/filterapi.h>

#include <string>
#include <fstream>

#include <zfs/layer.h>
#include <zfs/layer_impl.h>

#include <zfsfilter/tools/scan_measurement_data_layers.h>
#include "plane_parameter_matrix.h"

namespace zfsfilter
{
    namespace planes
    {
        // ======================================================================================================================

        using ptr_ZFLayerBit = std::shared_ptr<ZFLayerBit>;
        using ptr_ZFLayerDouble = std::shared_ptr<ZFLayerDouble>;
        using ptr_ZFLayerVec3d = std::shared_ptr<ZFLayerVec3d>;

        // ======================================================================================================================
        // class for reading/writing debug files for plane segmentation/integration
        // ======================================================================================================================

        class FILTERDLL_API plane_finder_measurement_data_layers
        {
        public:
            plane_finder_measurement_data_layers() = delete;
            plane_finder_measurement_data_layers( const std::wstring& scanFn_,
                                                  int   scan_section_identifier_ = 0,
                                                  bool enable_data_write_= false,
                                                  bool enable_auxilliary_data_write_= false);

            bool write_data(const plane_parameter_matrix_t& plane_data, bool bGenerateImages);
            bool read_data(plane_parameter_matrix_t& plane_data);
            
            bool write_threshold_parameter(const plane_parameter_matrix_t& plane_data);
            bool write_threshold_parameter(const plane_thresholds_t& plane_thresholds, const plane_connect_thresholds_t& plane_connect_thresholds);
            
            void read_double_parameter_from_fstream(std::fstream& file, double& parameter);
            void read_uint32_parameter_from_fstream(std::fstream& file, uint32_t& parameter);
            bool read_threshold_parameter(plane_thresholds_t& plane_thresholds, plane_connect_thresholds_t& plane_connect_thresholds);

        private:

            bool write_enabled;
            bool auxilliary_data_write_enabled;

            wstring data_in_out_directory;

            wstring thresholds_ini_filepath;

            scan_data_layer_path_generator_t scan_data_layer_path_generator;

            scan_measurement_data_layers_t scan_measurement_data_layers;
            scan_measurement_data_layers_t scan_measurement_auxilliary_data_layers;

            ptr_ZFLayerBit plane_valid_layer;

            ptr_ZFLayerVec3d normal_layer, normal_error_layer, normal_error_electronic_layer, normal_fit_plane_layer;
            ptr_ZFLayerVec3d center_position_layer, center_position_error_layer, center_position_fit_plane_layer, position_error_electronic_layer;
            ptr_ZFLayerVec3d position_error_measured_layer, plane_center_sample_position_layer, sample_coordinates_mean_position_layer;
            ptr_ZFLayerVec3d plane_tangent_x_layer, plane_tangent_y_layer, plane_tangent_x_error_layer, plane_tangent_y_error_layer;

            ptr_ZFLayerDouble plane_center_azimuth_angle_layer, plane_center_elevation_angle_layer;
            ptr_ZFLayerDouble distance_error_measured_layer, distance_error_electronic_layer, perpendicular_point2plane_distance_variance_layer;
            ptr_ZFLayerDouble x_error_measured_layer, y_error_measured_layer, z_error_measured_layer;
            ptr_ZFLayerDouble incidence_angle_layer, plane_area_layer;
            ptr_ZFLayerDouble intensity_average_layer, point_plane_distance_threshold_layer;
            ptr_ZFLayerDouble plane_distance_to_coordinate_0_layer, plane_distance_to_coordinate_0_error_layer;
            ptr_ZFLayerDouble range_to_center_position_layer;

            void map_measurement_data_pointers();
            void map_auxilliary_measurement_data_pointers();

        };

        // ======================================================================================================================
    }
}
