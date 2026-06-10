#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>
#include <vector>
#include <memory>

#include <zfs/Math/stl/vector_op.h>

namespace zfsfilter
{
    namespace planes
    { 
        // ========================================================================================================================
        using vec = std::vector<double>;
        using pt_3d_t = vec;
        using vec_3d_t = vec;
        
        // ========================================================================================================================
        struct plane_sample_pt_t
        {
            bool valid{ false };

            size_t idx{ 0 }; // pt index

            uint32_t u{ 0 }, v{ 0 }; // < pixel positions

            pt_3d_t pt;
            vec_3d_t pt_normal;

            plane_sample_pt_t(){}
            plane_sample_pt_t(size_t idx_) : idx(idx_) {}
            plane_sample_pt_t(const pt_3d_t& pt_) : pt(pt_){}
            plane_sample_pt_t(const uint32_t& u_, const uint32_t& v_, const pt_3d_t& pt_) : u(u_), v(v_), pt(pt_){}
            plane_sample_pt_t(const uint32_t& u_, const uint32_t& v_, const pt_3d_t& pt_, const vec_3d_t& pt_normal_) : u(u_), v(v_), pt(pt_), pt_normal(pt_normal_){}
        
            void set(const pt_3d_t& pt_) { pt = pt_; }
        
            double& x() { return pt[0]; }
            double& y() { return pt[1]; }
            double& z() { return pt[2]; }
        
            const double& x() const { return pt[0]; }
            const double& y() const { return pt[1]; }
            const double& z() const { return pt[2]; }
        
            double& n_x() { return pt_normal[0]; }
            double& n_y() { return pt_normal[1]; }
            double& n_z() { return pt_normal[2]; }

            const double& n_x() const { return pt_normal[0]; }
            const double& n_y() const { return pt_normal[1]; }
            const double& n_z() const { return pt_normal[2]; }
        };
        // ========================================================================================================================
        //using plane_data_sample_vector_t = std::vector<plane_data_sample_t>;
        using plane_sample_pt_ptr_t = std::shared_ptr<plane_sample_pt_t>;
        using plane_sample_pt_ptr_vector_t = std::vector<plane_sample_pt_ptr_t>;
        // ========================================================================================================================
        using plane_contour_pt_t = plane_sample_pt_t;
        // ========================================================================================================================
        using plane_contour_pt_ptr_t = std::shared_ptr<plane_contour_pt_t>;
        using plane_contour_pt_ptr_vector_t = std::vector<plane_contour_pt_ptr_t>;
        // ========================================================================================================================
        struct plane_pt_with_error_information_t: public plane_sample_pt_t
        {
            double perp_fit_variance{ 0 }; //
            double mean_distance_error{ 0 }; // variance

            vec_3d_t pt_error;
            vec_3d_t pt_normal_error;
        
            plane_pt_with_error_information_t(const uint32_t& u_, const uint32_t& v_, const pt_3d_t& pt_, const vec_3d_t& pt_error_)
                : plane_sample_pt_t(u_, v_, pt_), pt_error(pt_error_)
            {}

            plane_pt_with_error_information_t( const uint32_t& u_, const uint32_t& v_,
                                               const pt_3d_t& pt_, const vec_3d_t& pt_error_,
                                               const vec_3d_t& pt_normal_, const vec_3d_t& pt_normal_error_)
                                              : plane_sample_pt_t(u_, v_, pt_, pt_normal_), 
                                                pt_error(pt_error_), 
                                                pt_normal_error(pt_normal_error_)
            {
            
            }

            plane_pt_with_error_information_t( const uint32_t& u_, const uint32_t& v_,
                                               const pt_3d_t& pt_, const vec_3d_t& pt_error_,
                                               const vec_3d_t& pt_normal_, const vec_3d_t& pt_normal_error_,
                                               const double& perp_fit_variance_,
                                               const double& mean_distance_error_)
                                              : plane_sample_pt_t(u_, v_, pt_, pt_normal_), 
                                                perp_fit_variance(perp_fit_variance_),
                                                mean_distance_error(mean_distance_error_),
                                                pt_error(pt_error_), 
                                                pt_normal_error(pt_normal_error_)
                                                
            {}

            double& x_e() { return pt_error[0]; }
            double& y_e() { return pt_error[1]; }
            double& z_e() { return pt_error[2]; }

            const double& x_e() const { return pt_error[0]; }
            const double& y_e() const { return pt_error[1]; }
            const double& z_e() const { return pt_error[2]; }

            double& n_x_e() { return pt_normal_error[0]; }
            double& n_y_e() { return pt_normal_error[1]; }
            double& n_z_e() { return pt_normal_error[2]; }

            const double& n_x_e() const { return pt_normal_error[0]; }
            const double& n_y_e() const { return pt_normal_error[1]; }
            const double& n_z_e() const { return pt_normal_error[2]; }

            double& get_perp_fit_variance() { return perp_fit_variance; };
            const double& get_perp_fit_variance() const { return perp_fit_variance; };

            double& get_mean_distance_error() { return mean_distance_error; };
            const double& get_mean_distance_error() const { return mean_distance_error; };
        };
        // ========================================================================================================================
        using plane_pt_with_error_information_vector_t     = std::vector<plane_pt_with_error_information_t>;
        using plane_pt_with_error_information_ptr_t        = std::shared_ptr<plane_pt_with_error_information_t>;
        using plane_pt_with_error_information_ptr_vector_t = std::vector<plane_pt_with_error_information_ptr_t>;
        // ========================================================================================================================
    }
}