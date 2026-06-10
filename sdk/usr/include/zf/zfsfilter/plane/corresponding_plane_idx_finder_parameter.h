#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <Common/def.h>
SWITCH_WARNINGS_OFF
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
SWITCH_WARNINGS_ON

#include <zfs/Math/math/def.h>
#include <zfs/Math/auxilliary/types.h>

#include <zfsfilter/plane/plane_finder_parameter.h>

namespace zfsfilter
{

    namespace planes
    {
        // ==================================================================================

        struct scan_region_selection_t
        {
            using angle_range_t = std::vector<double>;

            enum class scan_typ_t: uint32_t {single_layer,dual_layer};

            angle_range_t azimuth_angle_range;  
            angle_range_t elevation_angle_range; 

            scan_region_selection_t()
            {
                set(scan_typ_t::single_layer);// << preset standard: single layer scan
            }

            scan_region_selection_t(scan_typ_t scan_typ)
            {
                set(scan_typ);
            }

            scan_region_selection_t(const angle_range_t& azimuth_angle_range_, const angle_range_t& elevation_angle_range_)
            {
                set(azimuth_angle_range_, elevation_angle_range_);
            }

            void set(scan_typ_t scan_typ)
            {
                switch (scan_typ)
                {
                    case scan_typ_t::single_layer:
                    {
                        set_azimuth_angle_range({ 0, PI });
                        break;
                    }
                    
                    case scan_typ_t::dual_layer:
                    {
                        set_azimuth_angle_range({ 0, 2*PI });
                        break;
                    }
                }

                set_elevation_angle_range({ 0, 2 * PI });
            }

            void set_azimuth_angle_range(const angle_range_t& azimuth_angle_range_)
            {
                azimuth_angle_range = azimuth_angle_range_;
            }

            void set_elevation_angle_range(const angle_range_t& elevation_angle_range_)
            {
                elevation_angle_range = elevation_angle_range_;
            }

            void set(const angle_range_t& azimuth_angle_range_, const angle_range_t& elevation_angle_range_)
            {
                set_azimuth_angle_range(azimuth_angle_range_);
                set_elevation_angle_range(elevation_angle_range_);
            }

            bool operator()(const double& azimuth_angle, const double& elevation_angle) const
            {
                return check(azimuth_angle, elevation_angle);
            }

            bool check( const double& azimuth_angle, const double& elevation_angle) const
            {
                return value_within_range_check(azimuth_angle_range[0], azimuth_angle_range[1], azimuth_angle) &&
                    value_within_range_check(elevation_angle_range[0], elevation_angle_range[1], elevation_angle);
            }

        private:

            bool value_within_range_check(const double& lower, const double& upper, const double& value) const
            {
                return value >= lower && value <= upper;
            }

        };

        // ==================================================================================

        using scan_region_selection_vector_t = std::vector< scan_region_selection_t>;

        struct multi_scan_region_selection_t
        {
            multi_scan_region_selection_t()
            {}

            multi_scan_region_selection_t(const scan_region_selection_vector_t& scan_region_selection_vector_)
            {
                set(scan_region_selection_vector_);
            }

            void set(const scan_region_selection_vector_t& scan_region_selection_vector_)
            {
                scan_region_selection_vector = scan_region_selection_vector_;
            }

            std::vector<int> operator()(const double& azimuth_angle, const double& elevation_angle) const
            {
                std::vector<int> ScanSections;
                for (size_t i = 0; i < scan_region_selection_vector.size(); ++i)
                {
                    if (scan_region_selection_vector[i](azimuth_angle, elevation_angle))
                        ScanSections.push_back(i);
                }
                return ScanSections;
            }

        private:

            scan_region_selection_vector_t scan_region_selection_vector;
        };

        // ==================================================================================

        struct corresponding_plane_idx_finder_parameter_t
        {

            // definitions ------------------------------------------------------------

            struct data_generation_options_t
            {
                bool write_debug_output{ false };
                bool generate_largest_corresponding_planes_indicating_matrices{ false };
                bool generate_corresponding_planes_normal_difference_angles{ false };
                
            };

            enum class plane_comparison_option_t: uint32_t {normals,normals_and_center_positions};

            // member variables -------------------------------------------------------
            data_generation_options_t data_generation_options;

            uint32_t N_largest{ 100 }; // < number of largest planes to check
            
            plane_comparison_option_t plane_comparison_option{ plane_comparison_option_t::normals };

            double idx_cnt_comparision_ratio_threshold{ 0.90 }; // < comparison of plane size in number of pixels 

            double angle_search_radius_normal{TORAD(7.5)}; // < search range angle for normal [rad]
            double position_search_radius_center_position{0.5}; // < search range for normal [m]

            multi_scan_region_selection_t scan_region_selection_vector_planes{ {scan_region_selection_t({0,PI},{0,2 * PI})}  };

            plane_connector_parameter_t::collected_plane_parameter_retrieval_type_t collected_plane_parameter_retrieval_type{ plane_connector_parameter_t::collected_plane_parameter_retrieval_type_t::fit };

            // member functions _------------------------------------------------------
            corresponding_plane_idx_finder_parameter_t()
            {
                init( data_generation_options_t(),
                      plane_comparison_option,
                      N_largest,
                      idx_cnt_comparision_ratio_threshold,
                      angle_search_radius_normal,
                      position_search_radius_center_position,
                      scan_region_selection_vector_planes,
                      collected_plane_parameter_retrieval_type);
            }

            corresponding_plane_idx_finder_parameter_t(
                const data_generation_options_t& data_generation_options_,
                const plane_comparison_option_t& plane_comparison_option_,
                const uint32_t& N_largest_,
                const double& idx_cnt_comparision_ratio_threshold_,
                const double& angle_search_radius_normal_,
                const double& position_search_radius_center_position_,
                const multi_scan_region_selection_t& scan_region_selection_vector_planes_,
                const plane_connector_parameter_t::collected_plane_parameter_retrieval_type_t& collected_plane_parameter_retrieval_type_)
            {
                init( data_generation_options_,
                      plane_comparison_option_,
                      N_largest_, 
                      idx_cnt_comparision_ratio_threshold_, 
                      angle_search_radius_normal_,
                      position_search_radius_center_position_,
                      scan_region_selection_vector_planes_,
                      collected_plane_parameter_retrieval_type_);
            }

            // ------------------------------------------------------------------------
            void init( const data_generation_options_t& data_generation_options_,
                       const plane_comparison_option_t& plane_comparison_option_,
                       const uint32_t& N_largest_,
                       const double& idx_cnt_comparision_ratio_threshold_,
                       const double& angle_search_radius_normal_,
                       const double& position_search_radius_center_position_,
                       const multi_scan_region_selection_t& scan_region_selection_vector_planes_,
                       const plane_connector_parameter_t::collected_plane_parameter_retrieval_type_t& collected_plane_parameter_retrieval_type_)
            {
                data_generation_options = data_generation_options_;

                N_largest = N_largest_;

                plane_comparison_option = plane_comparison_option_;

                idx_cnt_comparision_ratio_threshold = idx_cnt_comparision_ratio_threshold_;

                angle_search_radius_normal = angle_search_radius_normal_;

                position_search_radius_center_position = position_search_radius_center_position_;

                scan_region_selection_vector_planes = scan_region_selection_vector_planes_;

                collected_plane_parameter_retrieval_type = collected_plane_parameter_retrieval_type_;
            }

            std::string print_parameter() const
            {
                std::string log;

                log += "------------- corresponding plane finder idx parameters-------------\n";
                log += boost::str(boost::format("plane comparison option (0: normals, normals_and_center_positions: 1): %1%\n") % toUType(plane_comparison_option));
                log += boost::str(boost::format("angle search radius for plane comparison: %1% [rad]\n") % angle_search_radius_normal);
                log += boost::str(boost::format("center position difference search radius for plane comparison: %1% [m]\n") % position_search_radius_center_position);
                log += boost::str(boost::format("plane pixel numbers comparison ration: %1%\n") % idx_cnt_comparision_ratio_threshold);
                log += boost::str(boost::format("maximum number (largest) planes to examine: %1%\n") % N_largest);
                log += boost::str(boost::format("use collected plane parameter from retrieval type (0: averaging, 1: fit): %1%\n") % toUType(collected_plane_parameter_retrieval_type));
                log += "-------------------------------------------------\n";

                return log;
            }
        };

        // ==================================================================================

    }
}
