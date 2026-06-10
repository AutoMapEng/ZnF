#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>
#include <cmath>
#include <vector>
#include <algorithm>
#include <utility>
#include <vector>
#include <fstream>

#include <Common/def.h>

SWITCH_WARNINGS_OFF
#include <boost/thread.hpp>
SWITCH_WARNINGS_ON

#include <zfcore/filename.h>

#include <zfs/Math/math/def.h>
#include <zfs/Math/math/basic.h>
#include <zfs/Math/fit/surfacefit_xyz.h>

#include <zfsfilter/tools/scan_parameter.h>
#include <zfsfilter/tools/noise_parameter.h>
#include <zfsfilter/tools/scan_data_layer_path_generator.h>
#include <zfsfilter/tools/time_measurement.h>

#include <zfsfilter/plane/base_vector_definitions.h>
#include <zfsfilter/plane/plane_thresholds.h>
#include <zfsfilter/plane/plane_connect_thresholds.h>
#include <zfsfilter/plane/plane_connector_parameter.h>

namespace zfsfilter
{
    namespace planes
    {
        // ======================================================================================================================
        using namespace zfs::math;
        // ======================================================================================================================
        // defaults for 
        struct FILTERDLL_API planefinder_output_data_generation_settings_t
        {
            // --------------------------------------------------------------------------------------------------------------------------------------------
            
            bool  generate_valid_plane_tiles_mask{ false }; // < generate new mask
            bool  write_valid_plane_tiles_mask{ false }; // < write on existing mask

            bool  generate_valid_plane_outlier_tiles_mask{ false }; // < generate new mask
            bool  write_valid_plane_outlier_tiles_mask{ false }; // < write on existing mask

            bool  generate_valid_subplane_tiles_mask{ false }; // < generate new mask
            bool  write_valid_subplane_tiles_mask{ false }; // < write on existing mask

            bool  generate_data_tiling_indicator_mask{ false }; // < generate new mask
            bool  write_data_tiling_indicator_mask{ false }; // < write on existing mask

            bool  generate_measurement_data_layers{ false }; // main layer files (.zfl-format), processing parameters and thresholds file are written: debug/test 
            bool  generate_auxilliary_measurement_data_layers{ false }; // < auxiliary layer files are written generate_measurement_data_layers must be enabled: debug/algorithm test
            bool  generate_images_of_measurement_data_layers{ false }; // images of measurement data layers, tbi: debug/test 

            scan_section_identifier_t scan_section_identifier{ 0 }; // < up to now numeric; scan section identifier for measurement data layer path organization

            bool  generate_connected_plane_layer{ false }; // debug setting for writing the plane idx file (.zfl format)
            bool  generate_connected_plane_meta_data_file{ false }; // debug setting for writing the plane parameter data and convex hull data of planes to an output ascii file

            bool  generate_output_log_file{ false }; // < logs processing of scan, for debug, large step time measurement
            bool  generate_detailed_algorithm_timing_log{ false }; // < writen to output log file, detailed time measurement of algorithm steps in find_planes - routine
            bool  confirm_log_file_output{ false }; // < confirms succesful log-file output to current log screen.

            // --------------------------------------------------------------------------------------------------------------------------------------------

            bool output_masks_to_file() const;
            bool output_measurement_data_layers() const;

            bool write_all_valid_subplane_tiles_mask() const;
            
            bool write_plane_tiles_mask() const;
            bool write_outlier_plane_tiles_mask() const;

            bool write_data_tiling_mask() const;

            // --------------------------------------------------------------------------------------------------------------------------------------------

            enum class data_output_pre_settings_t : uint32_t {base_test, extended_output_test, develop, laser_control, calibration};

            void set_for(const data_output_pre_settings_t& data_output_pre_settings);

        protected:

            void set_for_base_test();
            void set_for_extended_output_test();
            void set_for_laser_control();
            void set_for_develop();
            void set_for_calibration();

            void switch_off_all();

            // --------------------------------------------------------------------------------------------------------------------------------------------

        };
        // ======================================================================================================================
        struct FILTERDLL_API planefinder_operation_settings_t
        {
            // --------------------------------------------------------------------------------------------------------------------------------------------
            
            // -- data input options (mainly for debugging purposes)
            bool increment_unset_vector_data_counts{ false }; // < for debug/algorithm test

            // -- handling of normal directions in subplane parameters
            bool harmonize_normals{ true }; // < normals are all looking away/towards co-ordinates center, otherwise normals are left unchanged (see also switch: normals_looking_away_from_coordinate_center)
            bool generate_normals_looking_away_from_coordinate_center{ false }; // < setting of the normal vector direction: true: away, false: towards co-ordinates center

            // -- collected plane data
            plane_connector_parameter_t::collected_plane_parameter_retrieval_type_t plane_parameter_retrieval_type{ plane_connector_parameter_t::collected_plane_parameter_retrieval_type_t::fit };
            plane_connector_parameter_t::plane_collection_comparison_operation_type_t plane_collection_comparison_operation_type{ plane_connector_parameter_t::plane_collection_comparison_operation_type_t::subplane2subplane/*collectedplane*/};

            bool calculate_plane_distance_for_collected_planes{ true }; // < calculate plane distance to co-ordinate center 0 for collected planes
            bool collect_extended_plane_data{ false }; // << ( get <collected> plane tangents(....for display))
                                                       // < planefinder_output_data_generation_settings_t.generate_auxilliary_measurement_data_layers
                                                       // < must be activated
            bool collect_plane_edge_points{ true }; // collect plane edge points

            bool calculate_convex_hull{ false };    // < calculate convex hull for collected planes
            bool calculate_plane_area_from_convex_hull{ false }; // < calculate plane area from convex hull (might be more exact, when large subsampling ratios are used
            bool calculate_detailed_contour_or_convex_hull{ false }; // < calculate contour using the plane edge points (contours and convex hulls become more detailed)

            // -- post processing of data  
            
            bool use_plane_center_sample_pts_for_plane_fits{ false }; // < "default" is to use the positions for (u,v) = (0,0) of the parametric (sub)plane fits of the first stage of the plane finder
            bool calculate_high_precision_planes{ false }; // < only collected planes satisfying additional (more stringent) requirements are calculated

            bool generate_kd_tree{ false }; // < for debugging and/or timing check
            bool generate_sorted_data_table{ false }; // < for debugging and check of the found number of planes with properties larger than given thresholds

            bool determine_calibration_parameter_correction_values{ false }; // < calculate calibration parameter correction values in case of 2-layer scans (see use in module: LRV/PlugIns/Filter/plane_finding.cpp)
            bool write_parameter_result_short_style{ true }; // < write calibration correction values in short style, e.g. for evaluation with matlab
            bool write_corrected_parameter_back_to_scan_header{ false }; // < write corrected parameter back to scan header
            bool writeCorrectedParameterPatchINI{ false };

            // -- processed scan selection

            // default: all
            double azimuth_angle_range{ 2* PI }; // determines used angle range (user provided)

            double start_azimuth_angle{ 0 }; // determined from scan start angle
            double stop_azimuth_angle{ 0 }; // determined from start_azimuth_angle+ azimuth_angle_range

            double start_elevation_angle{ 0 };
            double stop_elevation_angle{ 2* PI };

            uint32_t start_line{ 0 }; // < will be set
            uint32_t start_pixel{ 0 }; // < will be set
            uint32_t stop_line{ 0 }; // < will be set
            uint32_t stop_pixel{ 0 }; // < will be set

            // -- operation settings regarding sub lines to be processed ( mainly for debug purposes and processing parts out ot the whole data )
            uint32_t process_start_sub_line_index{0/*47//29//168-1// 131*/ }; // < start sub line index ( of all subsampled plane tile lines, see parameter n_plane_matrix_sample_step_azimuth): 0 refers to the first line of plane tiles in scan
                                                    // e.g. for medium res scan with ~2500 lines per azimuth angle range of pi
                                                    // azimuth_angle_step_per_scan_line = pi/2500 = 1.257 mrad
                                                    // for plane_edge_length of 9: azimuth_angle_step_per_sub_plane = 9*pi/2500 = 11.31 mrad
                                                    // for a subsample angle plane_parameter_matrix_sampling_angle_rad = 0.05 a subsampling factor for the 
                                                    // plane tile matrix of 4(.421) is obtained.
            uint32_t n_process_sub_lines{ 0 }; // < number of sub lines to process ( of all subsampled plane tile lines, see parameter n_plane_matrix_sample_step_azimuth ): when left 0, all lines are processed

            uint32_t process_start_sub_pixel_index{0/*488// 87//83-1 //208 */ }; // < start sub pixel index
            uint32_t n_process_sub_pixel{ 0/*3 / 1 */ }; // < number of pixels to process; if 0 all pixels are processed

#if 1 //ndef _DEBUG
            int32_t n_threads{ static_cast<int>(boost::thread::hardware_concurrency()) }; // < number of threads to use
#else
            int32_t n_threads{ 1 }; // < number of threads to use
#endif

            // -- plane matrix operations / subsampling
            bool    parameter_matrix_cyclic_along_azimuth{ false };
            bool    parameter_matrix_cyclic_along_elevation{false};
            bool    subsample_plane_parameter_matrix{ false };
            tuple_d plane_parameter_matrix_sampling_angles_rad{ tuple_d(0.0,0.0) }; // < sample angles pair<>(azimuth,elevation) used to (sub)sample plane parameter matrix elements (allows not only subsampling, but also oversampling - use this option carefully)

            // derived/set
            double plane_matrix_sample_step_azimuth{ 1. };
            double plane_matrix_sample_step_elevation{ 1. };

            // --------------------------------------------------------------------------------------------------------------------------------------------
            bool process_patch_selection() const;
            // --------------------------------------------------------------------------------------------------------------------------------------------
            void set_use_processing_percentage(const double& percentage);
            // --------------------------------------------------------------------------------------------------------------------------------------------

            planefinder_operation_settings_t();

        };

        // ======================================================================================================================

        struct FILTERDLL_API planefinder_data_display_settings_t
        {
            // --------------------------------------------------------------------------------------------------------------------------------------------
            
            bool display_set{ false };
            bool display_square_plane_representants{ false };    // < for display
            bool display_contour{ false };                       // < for display
            bool display_convex_hull{ false };                   // < for display

            int32_t plane_disp_default_count{ 100 };                // < number of planes to show
            int32_t plane_disp_count{ plane_disp_default_count };   // < number of planes to show

            vec_rgb base_colors{ zfs::math::rgb(0, 255, 0), zfs::math::rgb(255, 0, 255) };
            vec_rgb display_colors_scan_sections;

            // --------------------------------------------------------------------------------------------------------------------------------------------

            enum class data_display_setting_options_t : uint32_t {no_display, default_display, laser_control}; 

            planefinder_data_display_settings_t() { set_disp_default(); }
            planefinder_data_display_settings_t(bool nodisp) { if (nodisp) set_no_disp();  else set_disp_default(); }
            planefinder_data_display_settings_t(int number_of_scan_sections) { set_disp_settings(number_of_scan_sections); }

            void init(int number_of_scan_sections);

            void set_for(const data_display_setting_options_t& data_display_setting_options);

            void set_no_disp();
            void set_disp_default();
            void set_disp_settings(int number_of_scan_sections);

            bool display_planes() const;
        };
        // ======================================================================================================================
        struct FILTERDLL_API planefinder_parameter_t
        {
            // --------------------------------------------------------------------------------------------------------------------------------------------

            // processing options
            enum class fit_data_input_option_t: uint32_t {CONSTANT_LOOK_ANGLE_SPACING, VARIABLE_LOOK_ANGLE_SPACING};
            enum class plane_fit_algorithm_option_t : uint32_t {OUTLIER_CHECK_AND_REFIT_ALL_POINTS, SINGLE_FIT_VALIDATE_ALL_POINTS, DUAL_FIT_MEASURE_VARIANCE};

            // scan parameters (set from outside/ used during algorithm)
            scan_parameter_ptr_t                          scan_parameter;// < basic scan parameter

            // processing parameters
            planefinder_operation_settings_t              operation_settings;
            planefinder_output_data_generation_settings_t output_data_generation_settings;
            planefinder_data_display_settings_t           data_display_settings;

            plane_thresholds_t                            plane_thresholds; 
            plane_connect_thresholds_t                    plane_connect_thresholds;

            noise_parameter_t                             noise_parameter;
           
            // defaults for the following parameter settings
            surface_fit::operation_t        default_surface_fit_operation_mode{ surface_fit::operation_t::STANDARD_OPERATION };
            surface_fit::type_t             default_surface_fit_type{ surface_fit::type_t::CHOLESKY_DECOMPOSITION };
            std::set<surface_fit::option_t> default_surface_fit_options{  }; // { surface_fit::option_t::DONT_CALCULATE_RESIDUALS, surface_fit::option_t::DONT_CALCULATE_CHISQR }
            fit_data_input_option_t         default_fit_data_input_option{ fit_data_input_option_t::VARIABLE_LOOK_ANGLE_SPACING };
            plane_fit_algorithm_option_t    default_plane_fit_algorithm_option{ plane_fit_algorithm_option_t::DUAL_FIT_MEASURE_VARIANCE };

            //
            surface_fit::operation_t        surface_fit_operation_mode{ default_surface_fit_operation_mode };
            surface_fit::type_t             surface_fit_type{ default_surface_fit_type };
            std::set<surface_fit::option_t> surface_fit_options{ default_surface_fit_options };
            fit_data_input_option_t         fit_data_input_option{ default_fit_data_input_option };
            plane_fit_algorithm_option_t    plane_fit_algorithm_option{ default_plane_fit_algorithm_option };

            int32_t plane_fit_subsample_step{ 1 }; // < data point subsample step within plane rectangle (default for medium resolution scan)
            int32_t plane_validation_subsample_step{ 1 }; // < data point subsample step within plane rectangle (default for medium resolution scan)
            int32_t plane_subsample_ratio{ 1 }; // < ratio of plane_validation_subsample_step and plane_fit_subsample_step (default for medium resolution scan)
            // base (sizes and plane_thresholds)
            int32_t n_plane_edge_base_length{ 9 }; // < base length w.r.t. medium resolution scan
            int32_t n_plane_edge_length{ n_plane_edge_base_length }; // < plane length/ number plane samples along one edge of the plane (not subsampled/in data set)
            uint32_t nu_plane_edge_length{ uint32_t(n_plane_edge_length) }; // < plane length/ number plane samples along one edge of the plane (not subsampled/in data set) (unsigned version)

            int32_t n_plane_edge_samples{ n_plane_edge_length }; // < number plane samples along one edge of the plane
            uint32_t nu_plane_edge_samples{ uint32_t(n_plane_edge_samples) }; // < number plane samples along one edge of the plane (unsigned version)

            int32_t n_total_plane_samples{ n_plane_edge_samples * n_plane_edge_samples }; // < total number of plane samples

            int32_t min_intensity{ 0 }; // minimum intensity for sample collection into subplanes (when <=0 set to min intensity from scan header)
            double min_range{ 0.3 }; // minimum range for sample collection into subplanes (when <=0 set to min range from scan header)

            double nadir_skip_half_angle_deg{ 20. }; // < nadir skip region angle in [deg]
            double nadir_skip_half_angle_rad{ 0 }; // < nadir skip region angle in [rad]

            double elevation_angle_border_after_nadir_rad{ 0 };// < [rad], elevation angle border after nadir
            double elevation_angle_border_before_nadir_rad{ 0 }; // < [rad], elevation angle border before nadir

            double zenith_skip_half_angle_deg{ 0.0 }; // < zenith skip region angle in [deg]
            double zenith_skip_half_angle_rad{ TORAD(zenith_skip_half_angle_deg) }; // < zenith skip region angle in [rad]

            double elevation_angle_border_before_zenith_rad{ 0 }; // < [rad], elevation angle border before zenith
            double elevation_angle_border_after_zenith_rad{ 0 };// < [rad], elevation angle border after zenith

            // derived
            bool b_subsample_fit{ false };
            bool b_subsample_plane_validation{ false };
            bool b_subsample_fit_on_grid_with_plane_validation_subsample{ false };

            vec_b plane_fit_sample_indicator;

            xyz_surface_fit::option_t xyz_surface_fit_option{ xyz_surface_fit::option_t::STANDARD_OPERATION}; // option depending on the number of outlier threshold (to avoid unnecessary calculations in fit object)

            // parameter for surface integral determinations
            double azimuth_angle_range_one_sided{ 0 };
            double elevation_angle_range_one_sided{ 0 };

            // for CONSTANT_UV_MODE: 
            vec eav, aav;

            // --------------------------------------------------------------------------------------------------------------------------------------------

            planefinder_parameter_t();
        
            planefinder_parameter_t( const std::wstring& scanFn,
                                     const planefinder_output_data_generation_settings_t& data_generation_settings,
                                     const planefinder_data_display_settings_t& data_display_settings);

            planefinder_parameter_t( planefinder_operation_settings_t              operation_settings_,
                                     planefinder_output_data_generation_settings_t output_data_generation_settings_,
                                     planefinder_data_display_settings_t           data_display_settings_,
                                     int32_t                                       plane_fit_subsample_step_,
                                     int32_t                                       plane_validation_subsample_step_,
                                     int32_t                                       n_plane_edge_length_,
                                     plane_thresholds_t                            plane_thresholds_= plane_thresholds_t(),
                                     plane_connect_thresholds_t                    plane_connect_thresholds_= plane_connect_thresholds_t(),
                                     scan_parameter_ptr_t                          scan_parameter_ptr_ = std::make_shared< scan_parameter_t>(),
                                     surface_fit::type_t                           surface_fit_type_= planefinder_parameter_t().default_surface_fit_type,
                                     fit_data_input_option_t                       fit_data_input_option_= planefinder_parameter_t().default_fit_data_input_option,
                                     plane_fit_algorithm_option_t                  plane_fit_algorithm_option_= planefinder_parameter_t().default_plane_fit_algorithm_option);

            // --- functions for preparation of processing  -----------------------------------------------------------------------------------------------

            // scan parameter settings
            bool set_scan_parameter(const std::wstring& scanFn);
            bool set_scan_parameter(zfs::zfslinebyline_ptr& lbl);

            // set subplane base processing parameter
            void set_subplane_base_parameter( const int32_t& plane_fit_subsample_step_,
                                              const int32_t& plane_validation_subsample_step_,
                                              const int32_t& n_plane_edge_length_);

            // for setting constant angle input vectors (CONSTANT_UV_MODE)
            void set_angle_data_vectors();

            // for using the plane finder to determine calibration parameter
            static void set_for_2_scan_parts_along_elevation(const planefinder_parameter_t& parameters, planefinder_parameter_t& parameters_layer0_processing, planefinder_parameter_t& parameters_layer1_processing);
            void set_for_2_scan_parts_along_azimuth(planefinder_parameter_t& parameters_layer0_processing, planefinder_parameter_t& parameters_layer1_processing);

            // set data skip half angles
            void set_skip_angles(const double& nadir_skip_half_angle_deg_, const double& zenith_skip_half_angle_deg_);
            // set nadir skip half angle
            void set_nadir_skip_half_angle(const double& nadir_skip_half_angle_deg_);
            // set zenith skip half angle
            void set_zenith_skip_half_angle(const double& zenith_skip_half_angle_deg_);

            // --- conditions for processing --------------------------------------------------------------------------------------------------------------

            // elevation angle conditions
            bool elevation_angle_not_in_zenith_condition(const double& elevation_angle) const;
            bool elevation_angle_not_in_nadir_condition(const double& elevation_angle) const;
            bool elevation_angle_condition(const double& elevation_angle) const;

            // rg and rf conditions
            bool rg_and_rf_condition(int32_t rf, const double& rg) const;

            // --- output processing parameters to string -------------------------------------------------------------------------------------------------

            string print_plane_finder_parameters(const wstring& scanFn);
            string print_scan_parameters();

            // --- output processing parameters to file ---------------------------------------------------------------------------------------------------

            static wstring parameter_file_name(const wstring& datasetname_, const uint32_t& scan_section_identifier);
            bool write_plane_finder_parameters(const wstring& scanFn, const uint32_t& scan_section_identifier);

            // --- plane connector parameter --------------------------------------------------------------------------------------------------------------

            plane_connector_parameter_t get_plane_connector_parameter() const;

            // --------------------------------------------------------------------------------------------------------------------------------------------

            bool calibration_parameter_correction_values_shall_be_determined() const;
            bool possible_to_determine_calibration_parameter_correction_values() const;
            bool determine_calibration_parameter_correction_values() const;

            // --------------------------------------------------------------------------------------------------------------------------------------------
                
            //protected:

            // --- internal initialization functions ------------------------------------------------------------------------------------------------------

            bool detect_planefinder_subsampling(const wstring& scanFn, const uint32_t base_window_size_, uint32_t& window_size_, uint32_t& subsampling_);

            void set_dependend_parameter();
            bool set_scan_dependent_parameter(bool scan_parameters_set);
            
            void set_standard_values(const uint32_t& n_plane_samples);
            void set_surface_fit_option_and_operation_parameters();
            void set_mask_operation_parameters();

            // generate subsample vector
            vec_b gen_subsample(int subsample_step); 

        };
        // ======================================================================================================================
        typedef std::shared_ptr<planefinder_parameter_t> planefinder_parameter_ptr_t;
        // ======================================================================================================================
        FILTERDLL_API std::ostream& operator<<(std::ostream& lhs, const planefinder_parameter_t::fit_data_input_option_t& option);
        // ======================================================================================================================
        FILTERDLL_API std::ostream& operator<<(std::ostream& lhs, const planefinder_parameter_t::plane_fit_algorithm_option_t& option);
        // ======================================================================================================================
        class two_section_parameters_t
        {
            // --------------------------------------------------------------------------------------------------------------------------------------------
            
            enum class partitioning_t: uint32_t {in_elevation, in_azimuth};

            std::vector<planefinder_parameter_t> partition_parameters;

            // --------------------------------------------------------------------------------------------------------------------------------------------

            two_section_parameters_t()= default;
            two_section_parameters_t(const planefinder_parameter_t& planefinder_parameter, const partitioning_t& partitioning);
        };
        // ======================================================================================================================        
   }
}