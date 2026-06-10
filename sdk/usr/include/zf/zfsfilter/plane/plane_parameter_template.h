#pragma once

#include <vector>
#include <utility>
#include <cmath>

#include <zfs/scaninfo.h>
#include <zfs/Math/zfvec.h>
#include <zfs/Math/math/average.h>
#include <zfs/Math/math/mean.h>
#include <zfs/Math/fit/fit_plane.h>
#include <zfs/Math/stl/vector_op.h>

#include <zfs/Math/object/state.h>
#include <zfs/Math/object/queue_item.h>
#include <zfs/Math/3d/plane_center_distance_calculator.h>
#include <zfs/Math/fit/surface_xyz_integral.h>
#include <zfs/Math/signal_processing/dft2.h>

#include <zfsfilter/tools/scan_parameter.h>
#include "base_vector_definitions.h"
#include "plane_validation_parameter.h"
#include "plane_data_vector_matrix.h"
#include "structure_detector.h"

namespace zfsfilter
{
    namespace planes
    { 
        // ======================================================================================================================

        using namespace zfs::math;
        using namespace zfsfilter;

        // ======================================================================================================================
        // Class for keeping plane parameters and "validating" the plane
        // ======================================================================================================================
        template<class plane_fit_type> class plane_parameter_template_t : public queue_item
        {
        
        public:

            // state enum
            enum class state_t { DONE_PLANE_FIT, VALIDATED_OUTLIER, VALIDATED_PLANE, PLANE_PARAMETER_SET, PLANE_AUXILLIARY_PARAMETER_SET };
            enum class parameter_state_t {
                                           INTENSITY_AVERAGE,
                                           PERP_POINT_FIT_VARIANCE,

                                           CENTER_POSITION, CENTER_POSITION_ERROR, 
                                           PLANE_CENTER_DISTANCE, PLANE_CENTER_DISTANCE_ERROR,
                                           
                                           NORMAL, NORMAL_ERROR , 
                                           INCIDENCE_ANGLE,

                                           PLANE_TANGENTS, PLANE_TANGENT_ERRORS,
                                           PLANE_AREA,
            
                                           PLANE_EDGE_POINTS
            };
        
            // variable indicating whether plane is valid or not
            bool valid{ false };
            bool center_distance_invalid{ false };
            bool plane_fit_failed{ false };
            bool plane_normal_looking_away_from_coordinate_center{ false }; // < set in set(.) - function, indicating direction of normal
            bool normal_was_negated{ false }; // indicates if normal was negated during setting
        
            // plane center distance calculation
            zfs::math::planes::plane_distance_calculator plane_center_distance_calculation;

            // threshold to the mean square error for plane validation (individual thresholds for x,y and z component)
            vec rg_projection;
            vec mean_square_error_thresh;
        
            uint32_t plane_center_line_idx { 0 };
            uint32_t plane_center_pixel_idx{ 0 };

            double plane_center_azimuth_angle{ 0 };
            double plane_center_elevation_angle{ 0 };

            vec plane_center_sample_position{ vec(3,0) }; // < 3d data sample point from scan (x,y,z)-co-ordinates. 
                                                          // < fitting to the plane center azimuth and elevation angle 
                                                          // < stored in plane_center_azimuth_angle & plane_center_elevation_angle

            // mean square error of fit (for x,y and z component)
            vec mean_square_error_fit{ vec(3,0) };
        
            // measured plane parameters
            double distance_error_electronic{ 0 }; // < expected distance error ( from electronic noise ) , variance 
            double distance_error_expected{ 0 }; // < expected distance error ( from fit, at plane center ) , variance 
            double distance_error_measured{ 0 }; // < measured distance error , variance

            double perp_point_fit_variance{ 0 }; // < 

            vec position_error_electronic{ vec(3,0) };// variance of (x,y,z) from electronic noise
            vec position_error_electronic_{ vec(3,0) };// variance of (x,y,z) from electronic noise

            vec position_error_measured{ vec(3,0) };// variance of (x,y,z) from fit

            double plane_area{ 0 }; // < approximate plane area

            double range_to_center_position{ 0 };

            vec coordinates_mean_position{ vec(3,0) }; // < mean of co-ordinates (x_,y_,z_) used in plane fit

            vec center_position{ vec(3,0) }; // < plane center position ( u= 0, v = 0)
            vec center_position_fit_planes{ vec(3,0) }; // < center position (fit_planes)
        
            vec normal{ vec(3,0) }; // < plane normal
            vec normal_fit_planes{ vec(3,0) }; // < plane normal (fit_planes)
        
            vec plane_tangent_x{ vec(3,0) }; // use of surface tangents is much easier and less numerical complex, than using surface normal
            vec plane_tangent_y{ vec(3,0) };

            vec_tuple plane_edge_points_angles{ vec_tuple(4, tuple_d(0,0) ) }; // angles of plane edge points
            vec_vec   plane_edge_points{ vec_vec(4,vec(3,0)) }; // edge Points of plane

            double incidence_angle{ 0 }; // incidence angle at plane center
            double cos_incidence_angle{ 0 };
        
            // errors to the measurements
            vec normal_error{ vec(3,0) }; // < variances
            vec normal_error_electronic{ vec(3,0) }; // < variances
        
            vec center_position_error{ vec(3,0) }; // < variances
        
            vec plane_tangent_x_error{ vec(3,0) }; // < variances
            vec plane_tangent_y_error{ vec(3,0) }; // < variances

             // data for plane validation
            double point_plane_distance_threshold{ 0 };
            uint32_t n_residuals_out{ 0 };
            zfs::math::mean average_intensity;
            double intensity_average{ 0 };
        
        public:

            // constructor
            plane_parameter_template_t(){}
        
            // enquire whether plane is valid
            bool is_valid() const
            {
                return valid;
            }
            bool is_near_horizon(const double& max_off_horizon_angle_)
            {
                return (abs(plane_center_elevation_angle - PI/2) < max_off_horizon_angle_) || (abs(plane_center_elevation_angle - 3*PI/2) < max_off_horizon_angle_);
            }

            void set_plane_fit_failed()
            {
                plane_fit_failed = true;
            }
            // validate plane according to it's outliers
            void validate_outliers(const plane_fit_type& plane_fit, const plane_thresholds_t& thresholds)
            {
                if (plane_fit.get_noutliers() > thresholds.n_outlier_max_thresh)
                    // not valid plane
                    valid = false;
                else if (check_state(state_t::DONE_PLANE_FIT))
                    // valid plane for now
                    valid = true;
        
                set_state(state_t::VALIDATED_OUTLIER);
            }
        
            void validate_plane_condition(
                plane_fit_type& plane_fit, 
                const vec_b& outlier, 
                const double& intensity_average,
                const plane_validation_parameter_t& plane_validation_parameter)
            {
                plane_thresholds_t& plane_thresholds = plane_validation_parameter.plane_thresholds;
        
                const vec surface_perpendicular_residuals = plane_fit.get_surface_perpendicular_residuals();
        
                const size_t N = surface_perpendicular_residuals.size();
       
                if (plane_thresholds.plane_validation_option == plane_thresholds_t::plane_validation_option_t::MANUAL_THRESHOLDING)
                {
                    point_plane_distance_threshold = plane_thresholds.linear_range_dependent_plane_threshold(plane_fit.distance(0, 0));
                }
                else
                {
                    if (plane_validation_parameter.plane_thresholds.plane_sample_thresholding_option == plane_thresholds_t::plane_sample_thresholding_option_t::INCIDENCE_ANGLE_DEPENDENT)
                    {
                        // determine derivations of surfaces to obtain the surface tangents and their errors
                        if (!plane_fit.find_state(surface_fit::state_t::SURFACE_DERIVATIONS_SET))
                            plane_fit.set_derivations();

                        point_plane_distance_threshold = plane_validation_parameter.calc_sample_validation_threshold_incidence_dependent(intensity_average, plane_fit(0, 0), plane_fit.normal(0., 0.));
                    }
                    else if (plane_validation_parameter.plane_thresholds.plane_sample_thresholding_option == plane_thresholds_t::plane_sample_thresholding_option_t::DISTANCE_AND_INCIDENCE_DEPENDENT)
                    {
                        // determine derivations of surfaces to obtain the surface tangents and their errors
                        if (!plane_fit.find_state(surface_fit::state_t::SURFACE_DERIVATIONS_SET))
                            plane_fit.set_derivations();

                        point_plane_distance_threshold = plane_validation_parameter.calc_sample_validation_threshold_incidence_dependent(intensity_average, plane_fit(0, 0), plane_fit.normal(0., 0.));
                    }
                    else // plane_sample_thresholding_option_t::DEFAULT
                    {
                        point_plane_distance_threshold = plane_validation_parameter.calc_sample_validation_threshold(intensity_average);
                    }
                }

                for (size_t n = 0; n < N; ++n)
                {
                    if (!outlier[n] && abs(surface_perpendicular_residuals[n]) > point_plane_distance_threshold)
                    {
                        n_residuals_out++;
                    }
                }

                if (n_residuals_out) 
                    valid = false;
                else if (check_state(state_t::DONE_PLANE_FIT))
                    // valid plane for now
                    valid = true;
        
                set_state(state_t::VALIDATED_PLANE); 
            }
        
            // used for single fit validate all points option of plane finding 
            void validate_plane_condition(
                plane_fit_type& plane_fit, 
                const double& intensity_average,
                const plane_validation_parameter_t& plane_validation_parameter)
            {
                plane_thresholds_t& plane_thresholds = plane_validation_parameter.plane_thresholds;

                if (plane_thresholds.plane_validation_option == plane_thresholds_t::plane_validation_option_t::MANUAL_THRESHOLDING)
                {
                    point_plane_distance_threshold = plane_thresholds.linear_range_dependent_plane_threshold(plane_fit.distance(0, 0));
                }
                else
                {
                    if (plane_validation_parameter.plane_thresholds.plane_sample_thresholding_option == plane_thresholds_t::plane_sample_thresholding_option_t::INCIDENCE_ANGLE_DEPENDENT)
                    {
                        // determine derivations of surfaces to obtain the surface tangents and their errors
                        if (!plane_fit.find_state(surface_fit::state_t::SURFACE_DERIVATIONS_SET))
                            plane_fit.set_derivations();

                        point_plane_distance_threshold = plane_validation_parameter.calc_sample_validation_threshold_incidence_dependent(intensity_average, plane_fit(0, 0), plane_fit.normal(0., 0.));
                    }
                    else if (plane_validation_parameter.plane_thresholds.plane_sample_thresholding_option == plane_thresholds_t::plane_sample_thresholding_option_t::DISTANCE_AND_INCIDENCE_DEPENDENT)
                    {
                        // determine derivations of surfaces to obtain the surface tangents and their errors
                        if (!plane_fit.find_state(surface_fit::state_t::SURFACE_DERIVATIONS_SET))
                            plane_fit.set_derivations();

                        point_plane_distance_threshold = plane_validation_parameter.calc_sample_validation_threshold_incidence_dependent(intensity_average, plane_fit(0, 0), plane_fit.normal(0., 0.));
                    }
                    else // plane_sample_thresholding_option_t::DEFAULT
                    {
                        point_plane_distance_threshold = plane_validation_parameter.calc_sample_validation_threshold(intensity_average);
                    }
                }

                plane_fit.detect_distance_residual_outliers(point_plane_distance_threshold);
                n_residuals_out = plane_fit.get_noutliers();

                if (n_residuals_out) 
                    valid = false;
                else if (check_state(state_t::DONE_PLANE_FIT))
                    // valid plane for now (no previous setting)
                    valid = true;

                set_state(state_t::VALIDATED_PLANE);
            }

            void set( const uint32_t& plane_center_line_idx_, const uint32_t& plane_center_pixel_idx_,
                      const double& plane_center_azimuth_angle_,
                      const double& plane_center_elevation_angle_,
                      const vec& plane_center_sample_position_, // for segmentation
                      const vec& coordinate_mean_position_, // for common plane fit on segment patches
                      plane_fit_type& plane_fit, 
                      const surface_xyz_integral& surface_xyz_integral_,
                      bool harmonize_normals,
                      bool harmonize_normals_towards_coordinate_center,
                      const PlaneFit& fit_plane_plane_fit= PlaneFit())
            {
                // plane center position in image/scan
                plane_center_line_idx = plane_center_line_idx_;
                plane_center_pixel_idx = plane_center_pixel_idx_;

                // "plane center" sample angle values
                plane_center_azimuth_angle = plane_center_azimuth_angle_;
                plane_center_elevation_angle = plane_center_elevation_angle_;

                // coordinate mean position
                coordinates_mean_position = coordinate_mean_position_;

                // "plane center" sample position co-ordinates
                plane_center_sample_position = plane_center_sample_position_;

                // plane center position and error
                center_position[0] = plane_fit.x(0., 0.);
                center_position[1] = plane_fit.y(0., 0.);
                center_position[2] = plane_fit.z(0., 0.);
        
                range_to_center_position = plane_fit.distance(0., 0.);

                center_position_error[0] = plane_fit.var_x(0., 0.);
                center_position_error[1] = plane_fit.var_y(0., 0.);
                center_position_error[2] = plane_fit.var_z(0., 0.);
        
                distance_error_expected = center_position_error[0] + center_position_error[1] + center_position_error[2];

                vec surface_perpendicular_residuals = plane_fit.get_surface_perpendicular_residuals();
                perp_point_fit_variance = zfs::math::sum(zfs::math::sqr(surface_perpendicular_residuals));

                // determine derivations of surfaces to obtain the surface tangents and their errors
                if (!plane_fit.find_state(surface_fit::state_t::SURFACE_DERIVATIONS_SET))
                    plane_fit.set_derivations();
        
                // plane normal and error
                normal[0] = plane_fit.n_x(0., 0.);
                normal[1] = plane_fit.n_y(0., 0.);
                normal[2] = plane_fit.n_z(0., 0.);
        
                normal_error[0] = plane_fit.var_nx(0., 0.);
                normal_error[1] = plane_fit.var_ny(0., 0.);
                normal_error[2] = plane_fit.var_nz(0., 0.);
        
                // plane distance and normal direction (Hesse plane)
                plane_center_distance_calculation.calc(center_position, center_position_error, normal, normal_error);

                if (harmonize_normals)
                    normal_was_negated = plane_center_distance_calculation.set_looking_direction(harmonize_normals_towards_coordinate_center, normal);

                // plane area
                plane_area = surface_xyz_integral_.integrate();

                cos_incidence_angle = abs(dot(normal, normalize_2(center_position)));

                set_state(state_t::PLANE_PARAMETER_SET);
            }

            void set_auxilliary( const plane_fit_type& plane_fit, const surface_xyz_integral& surface_xyz_integral_, const double& intensity_average_, const range_d& range_u, const range_d& range_v, const bool& collect_plane_edge_points)
            {
                using zfs::math::negate;

                intensity_average = intensity_average_;

                distance_error_measured = plane_fit.distance_chisq() / (plane_fit.ndata() - 1.); // < variance

                position_error_measured[0] = plane_fit.chisqr_x() / (plane_fit.ndata() - 1.);
                position_error_measured[1] = plane_fit.chisqr_y() / (plane_fit.ndata() - 1.);
                position_error_measured[2] = plane_fit.chisqr_z() / (plane_fit.ndata() - 1.);

                incidence_angle = acos(cos_incidence_angle);

                plane_tangent_x = plane_fit.d_dx(0., 0.);
                plane_tangent_y = plane_fit.d_dy(0., 0.);

                if (normal_was_negated)
                    plane_tangent_y = negate(plane_tangent_y);

                plane_tangent_x_error = plane_fit.var_d_dx(0., 0.);
                plane_tangent_y_error = plane_fit.var_d_dy(0., 0.);
                
                // plane edge points
                if (collect_plane_edge_points)
                {
                    plane_edge_points_angles[0] = tuple_d(range_u.second, range_v.second);
                    plane_edge_points_angles[1] = tuple_d(range_u.first , range_v.second);
                    plane_edge_points_angles[2] = tuple_d(range_u.first , range_v.first);
                    plane_edge_points_angles[3] = tuple_d(range_u.second, range_v.first);

                    plane_edge_points[0] = center_position + plane_tangent_x * range_u.second + plane_tangent_y * range_v.second; // (+,+)
                    plane_edge_points[1] = center_position + plane_tangent_x * range_u.first  + plane_tangent_y * range_v.second; // (-,+)
                    plane_edge_points[2] = center_position + plane_tangent_x * range_u.first  + plane_tangent_y * range_v.first;  // (-,-)
                    plane_edge_points[3] = center_position + plane_tangent_x * range_u.second + plane_tangent_y * range_v.first;  // (+,-)
                }

                set_state(state_t::PLANE_AUXILLIARY_PARAMETER_SET);
            }
            // ====================================================================

            void set_electronic(const noise_parameter_t& noise_parameter, const plane_connector_parameter_t::plane_collection_comparison_operation_type_t& plane_collection_comparison_operation_type)
            {
                distance_error_electronic = noise_parameter.noise.calcM(intensity_average);
                distance_error_electronic *= distance_error_electronic; // variance

                double coordinate_noise_factor = distance_error_electronic / range_to_center_position;

                position_error_electronic[0] = center_position[0] * coordinate_noise_factor;
                position_error_electronic[1] = center_position[1] * coordinate_noise_factor;
                position_error_electronic[2] = center_position[2] * coordinate_noise_factor;

                position_error_electronic[0] *= position_error_electronic[0]; // variance
                position_error_electronic[1] *= position_error_electronic[1]; // variance
                position_error_electronic[2] *= position_error_electronic[2]; // variance

                double expected_to_electronic_correction_factor = distance_error_electronic / distance_error_expected;

                normal_error_electronic[0] = normal_error[0] * expected_to_electronic_correction_factor;
                normal_error_electronic[1] = normal_error[1] * expected_to_electronic_correction_factor;
                normal_error_electronic[2] = normal_error[2] * expected_to_electronic_correction_factor;
            }

            // ====================================================================
            bool has_plane_id() const
            {
                return plane_id != 0;
            }
        
            void set_plane_id(const uint32_t id)
            {
                plane_id = id;
            }
            
            const uint32_t& get_plane_id() const
            {
                return plane_id;
            }
            // ====================================================================
            bool check_state(const state_t& state_) const
            {
                return plane_state.check_current_state(state_);
            }

            bool find_state(const state_t& state_) const
            {
                return plane_state.find_state(state_);
            }

            void set_state(const state_t& state_)
            {
                plane_state.set_current_state(state_);
            }
            // ====================================================================


        private:

            zfs::math::base_state<state_t> plane_state;
        
            uint32_t plane_id{ 0 }; // < plane id of the plane the plane params element is assigned to
            
        };  
        
        // ======================================================================================================================   
    }
}