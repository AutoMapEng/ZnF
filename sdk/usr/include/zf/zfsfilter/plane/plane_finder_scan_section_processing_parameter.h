#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>
#include <vector>
#include <utility>

#include <zfs/linebyline_smart.h>

#include <zfs/Math/math/basic.h>

#include "plane_finder_parameter.h"

namespace zfsfilter
{
    namespace planes
    {
        // ==============================================================================================
        using tuple_d = std::pair<double, double>;
        // ==============================================================================================
        enum class scan_section_preparation_result_t : uint32_t { no_error, scan_out_of_required_processing_section , scan_section_extend_to_small };
        // ==============================================================================================
        struct plane_finder_scan_section_processing_parameter_t
        {
            scan_section_preparation_result_t scan_section_preparation_result{ scan_section_preparation_result_t::no_error };

            int32_t scan_section_start_line{ 0 };
            int32_t scan_section_start_pixel{ 0 };

            int32_t n_sublines{ 0 };
            int32_t n_subpixels{ 0 };

            double stepsublines{ 0 };
            double stepsubpixel{ 0 };

            int32_t n_scan_start_lines_offset{0}; 
            int32_t n_scan_start_pixel_offset{0}; 

            int32_t scan_section_identifier{ 0 };

            planefinder_parameter_t& planefinder_parameter;
            planefinder_operation_settings_t& planefinder_operation_settings;
            scan_parameter_t& scan_parameter;

            plane_finder_scan_section_processing_parameter_t(planefinder_parameter_t& planefinder_parameter_) 
                : planefinder_parameter(planefinder_parameter_), 
                  planefinder_operation_settings(planefinder_parameter.operation_settings),
                  scan_parameter(*planefinder_parameter.scan_parameter)
            {
                const scan_parameter_ptr_t scan_parameter = planefinder_parameter.scan_parameter;

                scan_section_identifier = planefinder_parameter.output_data_generation_settings.scan_section_identifier;

                // processing parameter :

                get_scan_line_region( planefinder_operation_settings.azimuth_angle_range,
                                      planefinder_operation_settings.start_azimuth_angle, 
                                      planefinder_operation_settings.stop_azimuth_angle,
                                      planefinder_operation_settings.start_line, planefinder_operation_settings.stop_line);

                get_scan_pixel_region( planefinder_operation_settings.start_elevation_angle,
                                       planefinder_operation_settings.stop_elevation_angle,
                                       planefinder_operation_settings.start_pixel, planefinder_operation_settings.stop_pixel);
                
                if (scan_section_preparation_succeeded())
                {

                    if (planefinder_operation_settings.stop_line == 0)
                        planefinder_operation_settings.stop_line = scan_parameter->lines - 1;

                    if (planefinder_operation_settings.stop_pixel == 0)
                        planefinder_operation_settings.stop_pixel = scan_parameter->pixels - 1;

                    calculate_scan_section_processing_extend();

                    if(scan_section_preparation_succeeded())
                        calculate_scan_section_pixel_positions();
                }
                
            }

            bool scan_section_preparation_succeeded() const
            {
                return scan_section_preparation_result == scan_section_preparation_result_t::no_error;
            }

            scan_section_preparation_result_t get_scan_section_preparation_result() const
            { 
                return scan_section_preparation_result;
            };

            std::string scan_section_preparation_error_msg() const
            {
                std::string error_message;

                switch (scan_section_preparation_result)
                {
                    case scan_section_preparation_result_t::no_error:
                    {
                        error_message = "no error occured.";
                        break;
                    }
                    case scan_section_preparation_result_t::scan_out_of_required_processing_section:
                    {
                        error_message = "scan out of required processing section!";
                        break;
                    }
                    case scan_section_preparation_result_t::scan_section_extend_to_small:
                    {
                        error_message = "scan section extend to small!";
                        break;
                    }
                    default:
                    {
                        error_message = "unknown error occured!";
                    }
                }

                return error_message;
            }

            bool lines_are_profiles() const
            {
                return true;
            }

            bool scan_section_cyclic_in_azimuth_direction() const
            {
                if (given_processing_azimuth_angle_range_to_process > 0)
                {
                    int cycles = static_cast<int>(given_processing_azimuth_angle_range_to_process / PI);

                    
                    return given_processing_azimuth_angle_range_to_process > ((cycles + 1.) * PI - 1.5 * plane_azimuth_angle_extend) ||
                        given_processing_azimuth_angle_range_to_process < (cycles* PI + 1.5 * plane_azimuth_angle_extend);
                }
                return false;
            }

            bool scan_section_cyclic_in_elevation_direction() const
            {
                if (given_processing_elevation_angle_range_to_process > 0)
                {
                    int cycles = static_cast<int>(given_processing_elevation_angle_range_to_process / (2 * PI));


                    return given_processing_elevation_angle_range_to_process > ((cycles + 1.) * (2 * PI) - 1.5 * plane_elevation_angle_extend) ||
                        given_processing_elevation_angle_range_to_process < (cycles* (2 * PI) + 1.5 * plane_elevation_angle_extend);
                }
                return false;
            }


            bool lines_cyclic() const
            {
                return lines_are_profiles() ? scan_section_cyclic_in_azimuth_direction() : scan_section_cyclic_in_elevation_direction();
            }

            bool pixels_cyclic() const
            {
                return lines_are_profiles() ? scan_section_cyclic_in_elevation_direction() : scan_section_cyclic_in_azimuth_direction();
            }


        private:

            double given_processing_azimuth_angle_range_to_process{ 0 };
            double given_processing_elevation_angle_range_to_process{ 0 };

            double plane_azimuth_angle_extend{ 0 };
            double plane_elevation_angle_extend{ 0 };

            double processing_azimuth_angle_range{ 0 };
            double processing_elevation_angle_range{ 0 };

            void calculate_scan_section_processing_extend()
            {
                given_processing_azimuth_angle_range_to_process   = planefinder_operation_settings.stop_azimuth_angle - planefinder_operation_settings.start_azimuth_angle;
                given_processing_elevation_angle_range_to_process = planefinder_operation_settings.stop_elevation_angle - planefinder_operation_settings.start_elevation_angle;

                plane_azimuth_angle_extend   = planefinder_parameter.nu_plane_edge_length * scan_parameter.azimuth_angle_step_per_line_rad;
                plane_elevation_angle_extend = planefinder_parameter.nu_plane_edge_length * scan_parameter.elevation_angle_step_per_pixel_rad;

                n_sublines = zfs::math::floor( ( given_processing_azimuth_angle_range_to_process   - plane_azimuth_angle_extend   ) / planefinder_parameter.operation_settings.plane_parameter_matrix_sampling_angles_rad.first) + 1;
                n_subpixels = zfs::math::floor( ( given_processing_elevation_angle_range_to_process - plane_elevation_angle_extend ) / planefinder_parameter.operation_settings.plane_parameter_matrix_sampling_angles_rad.second)+ 1;

                if (n_sublines != 0 && n_subpixels != 0)
                {
                    processing_azimuth_angle_range = (n_sublines - 1.) * planefinder_parameter.operation_settings.plane_parameter_matrix_sampling_angles_rad.first + plane_azimuth_angle_extend;
                    processing_elevation_angle_range = (n_subpixels - 1.) * planefinder_parameter.operation_settings.plane_parameter_matrix_sampling_angles_rad.second + plane_elevation_angle_extend;
                }
                else
                {
                    scan_section_preparation_result = scan_section_preparation_result_t::scan_section_extend_to_small;
                }
            }

            void calculate_scan_section_pixel_positions()
            {

                double offset_to_start_azimuth_angle = planefinder_operation_settings.start_azimuth_angle - scan_parameter.first_low_encoder_angle_rad; // given by user
                double offset_to_start_elevation_angle = planefinder_operation_settings.start_elevation_angle - scan_parameter.elevation_angle_of_first_pixel_rad;

                double azimuth_angle_margin = (given_processing_azimuth_angle_range_to_process - processing_azimuth_angle_range)/2.;
                double elevation_angle_margin = (given_processing_elevation_angle_range_to_process - processing_elevation_angle_range)/2.;

                n_scan_start_lines_offset = zfs::math::floor( 

                        // offset to start azimuth angle (+)
                        // centering of effective processing area in given processing range  (+)
                        // -> devided by azimuth line sampling distance angle

                        (offset_to_start_azimuth_angle + azimuth_angle_margin) / scan_parameter.azimuth_angle_step_per_line_rad

                    )+ 1; // -> floor(.) + 1 to get start line (ceil(.) is not used, since it gives for integer value not the next integer)

                n_scan_start_pixel_offset =
                    zfs::math::floor(

                        // offset to start elevation angle (+)
                        // centering of effective processing area in given processing range  (+)
                        // -> devided by elevation line sampling distance angle

                        (offset_to_start_elevation_angle + elevation_angle_margin) / scan_parameter.elevation_angle_step_per_pixel_rad
                        
                    )+ 1; // -> floor(.) + 1 to get start pixel (ceil(.) is not used, since it gives for integer value not the next integer)

                // subsampling of data by given plane sampling angle
                planefinder_parameter.operation_settings.plane_matrix_sample_step_azimuth  = planefinder_parameter.operation_settings.plane_parameter_matrix_sampling_angles_rad.first/ scan_parameter.azimuth_angle_step_per_line_rad;
                planefinder_parameter.operation_settings.plane_matrix_sample_step_elevation= planefinder_parameter.operation_settings.plane_parameter_matrix_sampling_angles_rad.second/ scan_parameter.elevation_angle_step_per_pixel_rad;

                stepsublines = planefinder_parameter.operation_settings.plane_matrix_sample_step_azimuth;
                stepsubpixel = planefinder_parameter.operation_settings.plane_matrix_sample_step_elevation;
            }

            // -- helping functions --------------------------------------------------------------------------------------------------------------

            // the scan line region is aligned with the scan start
            void get_scan_line_region( double& azimuth_angle_range_to_process,
                                       double& start_azimuth_angle_rad, double& stop_azimuth_angle_rad,
                                       uint32_t& start_line, uint32_t& stop_line)
            {
                start_azimuth_angle_rad = scan_parameter.first_low_encoder_angle_rad;
                stop_azimuth_angle_rad = scan_parameter.first_low_encoder_angle_rad+ azimuth_angle_range_to_process;

                if (stop_azimuth_angle_rad > scan_parameter.last_low_encoder_angle_rad)
                {
                    stop_azimuth_angle_rad = scan_parameter.last_low_encoder_angle_rad;
                }

                if (stop_azimuth_angle_rad < start_azimuth_angle_rad) stop_azimuth_angle_rad = start_azimuth_angle_rad;

                if (start_azimuth_angle_rad > scan_parameter.last_low_encoder_angle_rad || stop_azimuth_angle_rad < scan_parameter.first_low_encoder_angle_rad)
                    scan_section_preparation_result = scan_section_preparation_result_t::scan_out_of_required_processing_section;

                if (scan_section_preparation_succeeded())
                {
                    azimuth_angle_range_to_process = stop_azimuth_angle_rad - start_azimuth_angle_rad;

                    start_line = zfs::math::round((start_azimuth_angle_rad - scan_parameter.first_low_encoder_angle_rad) / scan_parameter.azimuth_angle_step_per_line_rad);
                    stop_line = zfs::math::round((stop_azimuth_angle_rad - scan_parameter.first_low_encoder_angle_rad) / scan_parameter.azimuth_angle_step_per_line_rad);

                    if (stop_line >= scan_parameter.lines)
                        stop_line = scan_parameter.lines - 1;
                }
                else
                {
                    start_line = 0;
                    stop_line = -1;
                }
            }

            // the scan pixel region is determined "absolutely" from the given parameters start_elevation_angle_rad and stop_elevation_angle_rad
            void get_scan_pixel_region( double& start_elevation_angle_rad, double& stop_elevation_angle_rad,
                                        uint32_t& start_pixel, uint32_t& stop_pixel)
            {
                if (stop_elevation_angle_rad < start_elevation_angle_rad) stop_elevation_angle_rad = start_elevation_angle_rad;

                if (start_elevation_angle_rad > scan_parameter.elevation_angle_of_last_pixel_rad || stop_elevation_angle_rad < scan_parameter.elevation_angle_of_first_pixel_rad)
                    scan_section_preparation_result = scan_section_preparation_result_t::scan_out_of_required_processing_section;

                if (scan_section_preparation_succeeded())
                {
                    if (start_elevation_angle_rad < scan_parameter.elevation_angle_of_first_pixel_rad)
                        start_elevation_angle_rad = scan_parameter.elevation_angle_of_first_pixel_rad;

                    if (stop_elevation_angle_rad > scan_parameter.elevation_angle_of_last_pixel_rad)
                        stop_elevation_angle_rad = scan_parameter.elevation_angle_of_last_pixel_rad;

                    start_pixel = zfs::math::round((start_elevation_angle_rad - scan_parameter.elevation_angle_of_first_pixel_rad) / scan_parameter.elevation_angle_step_per_pixel_rad);
                    stop_pixel = zfs::math::round((stop_elevation_angle_rad - scan_parameter.elevation_angle_of_first_pixel_rad) / scan_parameter.elevation_angle_step_per_pixel_rad);

                    if (stop_pixel >= scan_parameter.pixels)
                        stop_pixel = scan_parameter.pixels - 1;
                }
            }


        };
        // ==============================================================================================
        
   }
}