#pragma once

#include <cstdint>
#include <vector>

#include <zfs/Math/stl/vector_op.h>
#include <zfs/Math/statistics/average_3d.h>
#include <zfs/Math/statistics/mean_3d.h>
#include <zfs/Math/statistics/weighted_average_3d.h>

namespace zfs {
    namespace math {
        namespace planes {
            // ========================================================================================================================
            using vec = std::vector<double>;
            using vec_b = std::vector<bool>;
            // ========================================================================================================================
            // class for calculating plane center distance from normal and plane (center) position
            // ========================================================================================================================
            struct plane_distance_calculator
            {
                plane_distance_calculator() = default;

                void calc(const vec& position_on_plane_, const vec& normal_)
                {
                    using namespace zfs::math;

                    // plane equation: a*x+b*y+c*z+d= n*p+d= 0: d= -n*p0, p0 on the plane
                    // -> d is positive, when n is looking towards co-ordinate center
                    plane_distance_to_coordinate_center = -dot(normal_, position_on_plane_);
                    plane_normal_looking_away_from_coordinate_center = (plane_distance_to_coordinate_center < 0);
                }

                void calc(const vec& center_position_,
                    vec center_position_error_,
                    const vec& normal_,
                    vec normal_error_,
                    const bool& input_are_square_errors_ = true)
                {
                    using namespace zfs::math;

                    calc(center_position_, normal_);

                    vec normal_sqr = sqr(normal_);
                    vec center_position_sqr = sqr(center_position_);

                    if (!input_are_square_errors_)
                    {
                        normal_error_ = sqr(normal_error_);
                        center_position_error_ = sqr(center_position_error_);
                    }

                    plane_distance_to_coordinate_center_error = dot(normal_sqr, center_position_error_) + dot(center_position_sqr, normal_error_);
                }

                // returns true if normal is directed "outside", i.e. away from co-ordinate center O
                // (dot product of normal and given position is > 0)
                void calc(const average_3D& center_position_,
                    const average_3D& normal_, bool error = true)
                {
                    if (error)
                    {
                        calc(center_position_.getAvr(),
                            center_position_.getStd(),
                            normal_.getAvr(),
                            normal_.getStd(),
                            false);
                    }
                    else
                    {
                        calc(center_position_.getAvr(), normal_.getAvr());
                    }

                }

                // returns true if normal is directed "outside", i.e. away from co-ordinate center O
                // (dot product of normal and given position is > 0)
                template<class averager_t> void calc(const averager_t& center_position_, const mean_3D& center_position_error_,
                    const averager_t& normal_, const mean_3D& normal_error_, bool error = true)
                {
                    if (error)
                    {
                        calc(center_position_.getAvr(),
                            sqrt(center_position_error_.getAvr()),
                            normal_.getAvr(),
                            sqrt(normal_error_.getAvr()),
                            false);
                    }
                    else
                    {
                        calc(center_position_.getAvr(), normal_.getAvr());
                    }

                }

                bool set_looking_direction(bool away_from_coordinate_center, vec& normal) // used when normals are harmonized in their direction so that distances to planes are defined to be positive
                {
                    if (away_from_coordinate_center && !plane_normal_looking_away_from_coordinate_center)// redirect normal away from 0-point, if necessary (and adapt distance sign)
                    {
                        // turn normal outward
                        normal = negate(normal);
                        plane_distance_to_coordinate_center = -plane_distance_to_coordinate_center;
                        plane_normal_looking_away_from_coordinate_center = true;
                        return true;
                    }
                    else if (!away_from_coordinate_center && plane_normal_looking_away_from_coordinate_center) // redirect normals to 0-point, if necessary and adapt plane distances accordingly
                    {
                        // turn normal inward
                        normal = negate(normal);
                        plane_distance_to_coordinate_center = -plane_distance_to_coordinate_center;
                        plane_normal_looking_away_from_coordinate_center = false;
                        return true;
                    }
                    return false;
                }

                // -- parameter ---------------------------------------------------------------------------------------------
                bool plane_normal_looking_away_from_coordinate_center{ false };

                double plane_distance_to_coordinate_center{ 0 };
                double plane_distance_to_coordinate_center_error{ 0 };

            };
        } // end namespace planes
    } // end namespace math
} // end namespace zfs