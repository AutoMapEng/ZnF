#pragma once

#if 0

#include <Common/def.h>
SWITCH_WARNINGS_OFF
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
SWITCH_WARNINGS_ON
#include <zfcore/filename.h>

#include <cstdint>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <memory>

#include <zfs/zfscan_smart.h>
#include <zfs/layer.h>

#include <zfs/Math/math/def.h>
#include <zfs/Math/stl/vector_op.h>
#include <zfs/Math/statistics/histogram.h>

#include <zftools/zeitmessung.h>

#include <zfsfilter/Matrix.h>

#include <zfsfilter/tools/measurement_data_layer_path_generator.h>
#include <zfsfilter/plane/plane_finder_parameter.h>
#include <zfsfilter/plane/plane_finder_processing_result.h>

#include <zfsfilter/plane/corresponding_plane_idx_finder_parameter.h>

namespace zfsfilter
{

    namespace planes
    {

        // ==================================================================================

        //using namespace std;
        using namespace zfs::math;
        using namespace zftools;
        using namespace zfsfilter;

        // ==================================================================================
        struct value_comparison
        {
            value_comparison() {}

            static bool value_within_range_check(const double& range_center, const double& range_extend, const double& test_value)
            {
                return test_value >= (range_center - range_extend) && test_value <= (range_center + range_extend);
            }

            static bool value_within_range_check(const double& value_difference, const double& range_extend)
            {
                return abs(value_difference) < range_extend;
            }
        };
        // ==================================================================================
        struct plane_comparison_sigma: public value_comparison
        {
            plane_comparison_sigma() {}

            static bool plane_comparision_condition_one_sided(const corresponding_plane_idx_finder_parameter_t::plane_comparison_option_t& plane_comparison_option,

                const vec& test_center_position,
                const vec& center_position,
                const vec& center_position_std,

                const vec& test_normal,
                const vec& normal,
                const vec& normal_std,

                const double& sigma_multiplicator = 1.)
            {
                if (plane_comparison_option == corresponding_plane_idx_finder_parameter_t::plane_comparison_option_t::normals_and_center_positions)
                {
                    return value_within_range_check(center_position[0], sigma_multiplicator* center_position_std[0], test_center_position[0]) &&
                           value_within_range_check(center_position[1], sigma_multiplicator* center_position_std[1], test_center_position[1]) &&
                           value_within_range_check(center_position[2], sigma_multiplicator* center_position_std[2], test_center_position[2]) &&

                        value_within_range_check(normal[0], sigma_multiplicator* normal_std[0], test_normal[0]) &&
                        value_within_range_check(normal[1], sigma_multiplicator* normal_std[1], test_normal[1]) &&
                        value_within_range_check(normal[2], sigma_multiplicator* normal_std[2], test_normal[2] );
                }
                else
                {
                    return value_within_range_check(normal[0], sigma_multiplicator* normal_std[0], test_normal[0]) &&
                           value_within_range_check(normal[1], sigma_multiplicator* normal_std[1], test_normal[1]) &&
                           value_within_range_check(normal[2], sigma_multiplicator* normal_std[2], test_normal[2]);
                }
            }

            static bool plane_comparision_condition(const corresponding_plane_idx_finder_parameter_t::plane_comparison_option_t& plane_comparison_option,

                const vec& center_position_plane_0,
                const vec& center_position_std_plane_0,

                const vec& center_position_plane_1,
                const vec& center_position_std_plane_1,

                const vec& normal_plane_0,
                const vec& normal_std_plane_0,

                const vec& normal_plane_1,
                const vec& normal_std_plane_1,

                const double& sigma_multiplicator = 1.) 
            {
                bool bCondition =

                    plane_comparision_condition_one_sided(plane_comparison_option,
                        center_position_plane_0,
                        center_position_plane_1,
                        center_position_std_plane_1,
                        normal_plane_0,
                        normal_plane_1,
                        normal_std_plane_1,
                        sigma_multiplicator)

                    ||

                    plane_comparision_condition_one_sided(plane_comparison_option,
                        center_position_plane_1,
                        center_position_plane_0,
                        center_position_std_plane_0,
                        normal_plane_1,
                        normal_plane_0,
                        normal_std_plane_0,
                        sigma_multiplicator);

                return bCondition;
            }
        };

        // ==================================================================================
        struct plane_comparison_radius : public value_comparison
        {
            plane_comparison_radius() {}

            static bool plane_comparision_condition
            (
                const corresponding_plane_idx_finder_parameter_t::plane_comparison_option_t& plane_comparison_option,

                const vec& test_center_position,
                const vec& center_position,
                const double& center_position_comparison_radius,

                const vec& test_normal,
                const vec& normal,
                const double& normal_angle_radius                        
            )
            {
                if (plane_comparison_option == corresponding_plane_idx_finder_parameter_t::plane_comparison_option_t::normals_and_center_positions)
                {
                    return 
                        value_within_range_check(center_position[0], center_position_comparison_radius, test_center_position[0]) &&
                        value_within_range_check(center_position[1], center_position_comparison_radius, test_center_position[1]) &&
                        value_within_range_check(center_position[2], center_position_comparison_radius, test_center_position[2]) &&

                        value_within_range_check(zfs::math::angle(normal, test_normal), normal_angle_radius);
                }
                else
                {
                    return value_within_range_check(zfs::math::angle(normal, test_normal), normal_angle_radius);
                }
            }
        };

        // ==================================================================================

        struct corresponding_plane_idx_finder_t
        {
            // =======================================================================================================================

            using corresponding_index_t = std::pair<uint32_t, uint32_t>;
            using corresponding_plane_index_vector_t = std::vector< corresponding_index_t>;

            using corresponding_planes_vectors_t = std::pair<vec, vec>; 
            using corresponding_planes_vectors_vector_t = std::vector < corresponding_planes_vectors_t >;

            using corresponding_plane_normal_difference_angle_vector_t = std::vector< double >;

            using corresponding_plane_distance_to_coordinate_zero_t = std::pair<double, double>;
            using corresponding_plane_distance_to_coordinate_zero_vector_t = std::vector < corresponding_plane_distance_to_coordinate_zero_t >;

            using corresponding_plane_mean_angles_t = std::pair<vec, vec>;
            using corresponding_plane_mean_angles_vector_t = std::vector < corresponding_plane_mean_angles_t >;

            // =======================================================================================================================

            struct section_plane_correspondence_t
            {
                corresponding_plane_index_vector_t                       corresponding_plane_index_vector;

                corresponding_planes_vectors_vector_t                    corresponding_plane_normals_vector;
                corresponding_planes_vectors_vector_t                    corresponding_plane_normals_sigma_vector;

                corresponding_planes_vectors_vector_t                    corresponding_plane_centerpositions_vector;
                corresponding_planes_vectors_vector_t                    corresponding_plane_centerpositions_sigma_vector;

                corresponding_plane_mean_angles_vector_t                 corresponding_plane_mean_angles_vector;

                corresponding_plane_normal_difference_angle_vector_t     corresponding_plane_normal_difference_angle_vector;
                corresponding_plane_distance_to_coordinate_zero_vector_t corresponding_plane_distance_to_coordinate_zero_vector;
            };

            // =======================================================================================================================

            using layer_plane_correspondence_t        = std::pair<int32_t, int32_t>; // first: section 0 plane idx, second: section 1 plane idx, one of the indices == 0 => there exists no correponding plane in the other section
            using layer_plane_correspondence_vector_t = std::vector< layer_plane_correspondence_t >; // all correspondences found in one layer

            // =======================================================================================================================

            corresponding_plane_idx_finder_parameter_t parameter;

            plane_comparison_radius plane_comparison;

            uint32_t SectionPlaneIdxMatrixLines { 0 }; // < minimum common matrix size of section plane idx matrices
            uint32_t SectionPlaneIdxMatrixPixels{ 0 };

            matrix_uint32_ptr_t PlaneIdxSection0MatrixPtr; // < matrices which contain the collected/segmented planes
            matrix_uint32_ptr_t PlaneIdxSection1MatrixPtr;

            matrix_uint32_ptr_t PlaneIdxSection1FlippedMatrixPtr; // < flipped matrix PlaneIdxSection1Matrix

            Histogram<double, uint32_t> PlaneIdxSection0Histogram; // < index occurence histograms for segementation matrices 0 and 1
            Histogram<double, uint32_t> PlaneIdxSection1Histogram;

            const std::vector<uint32_t>& histogram0_frequencies; // < references to histogram frequencies vectors
            const std::vector<uint32_t>& histogram1_frequencies;

            std::vector<size_t> histogram0_frequencies_sort_idcs; // sort indices to frequencies in the histograms (descending)
            std::vector<size_t> histogram1_frequencies_sort_idcs;

            matrix_uint32_ptr_t IdxCorrespondenceMatrixPtr; // < matrix containing the frequencies (Häufigkeiten) of corresponding indices (sorted according to their frequencies in
                                                            // < the index histograms referenced by sort_idcs0 and sort_idcs1.

            matrix_uint32_ptr_t IdxInfoMatrixPtr; // < for check / visualization , contains frequency vectors of the histograms (sorted) and sort indices

            matrix_uint32_ptr_t CorrespPlaneIdxSection0MatrixPtr; // < for check / visualization, matrices containing the N_largest corresponding planes
            matrix_uint32_ptr_t CorrespPlaneIdxSection1MatrixPtr;

            section_plane_correspondence_t scan_section_correspondences; // object keeping the corresponding parameters accross sections

            layer_plane_correspondence_vector_t layer_plane_correspondence_vector_layer1, layer_plane_correspondence_vector_layer2;

            // =======================================================================================================================

            corresponding_plane_idx_finder_t()
                : histogram0_frequencies(PlaneIdxSection0Histogram.getHistogramFrequencies()), // get access to frequency vectors of histograms
                  histogram1_frequencies(PlaneIdxSection1Histogram.getHistogramFrequencies())
            {}

            // ==================================================================================

            string execute( const planefinder_parameter_t& plane_finder_parameter_,
                          const plane_finder_processing_result_t& plane_finder_processing_result_,
                          const corresponding_plane_idx_finder_parameter_t& corresponding_plane_idx_finder_parameter_)
            {
                string log;

                // start log
                log += "\n";
                log += "================= corresponding plane index finder ===============\n";
                log += "(finds corresponding planes in two sections of a two-section-scan)\n";
                log += "execution start: " + current_date_and_time_plus_weekday_and_timezone() + "\n";
                log += "==================================================================\n";

                zftools::TimeMeasurement<double, standard_period> time_take;

                parameter = corresponding_plane_idx_finder_parameter_;

                log += parameter.print_parameter();

                PlaneIdxSection0MatrixPtr = plane_finder_processing_result_.get_plane_idx_matrix(0);
                PlaneIdxSection1MatrixPtr = plane_finder_processing_result_.get_plane_idx_matrix(1);

                double duration_finding_idcs = 0;

                log += "------------------------------------------------------------------\n";
                log += "-- find corresponding planes --\n";

                if (find_corresponding_plane_index_values(plane_finder_processing_result_))
                {
                    duration_finding_idcs = time_take.duration2last();
                    log += "-- time needed for finding corresponding idcs: " + to_string(duration_finding_idcs) + " seconds. \n";
                }
                else
                {
                    log += ">>>>finding of corresponding idcs failed!<<<<";
                }
                log += "------------------------------------------------------------------\n";

                double duration_writing_debug_output= 0;

                if (parameter.data_generation_options.write_debug_output)
                {
                    log += "------------------------------------------------------------------\n";
                    log += "-- write debug output --\n";

                    if (write_debug_layers(plane_finder_parameter_.scan_parameter->scanFn.WString()))
                    {
                        duration_writing_debug_output = time_take.duration2last();
                        log += "-- time needed for writing debug output: " + to_string(duration_writing_debug_output) + " seconds. \n";
                    }
                    else
                    {
                        log += ">>>>writing of debug layers failed!<<<<";
                    }
                    log += "------------------------------------------------------------------\n";
                }

                log += boost::str(boost::format("--- total time needed: %1% seconds\n") % time_take.duration());

                log += "==================================================================\n";
                log += "execution end: " + current_date_and_time_plus_weekday() + "\n";
                log += "==================================================================\n";

                return log;
            }

            // ==================================================================================

            

            bool find_corresponding_plane_index_values( const plane_finder_processing_result_t& plane_finder_processing_result_)
            {
                using zfs::math::negate;

                if (!PlaneIdxSection0MatrixPtr || !PlaneIdxSection1MatrixPtr)
                    return false;

                // -- Input matrices
                SectionPlaneIdxMatrixLines = std::min(PlaneIdxSection0MatrixPtr->Lines(), PlaneIdxSection1MatrixPtr->Lines());
                SectionPlaneIdxMatrixPixels= std::min(PlaneIdxSection0MatrixPtr->Columns(), PlaneIdxSection1MatrixPtr->Columns());

                Matrix<uint32_t>& PlaneIdxSection0Matrix = *PlaneIdxSection0MatrixPtr;
                Matrix<uint32_t>& PlaneIdxSection1Matrix = *PlaneIdxSection1MatrixPtr;

                PlaneIdxSection1FlippedMatrixPtr = make_shared< Matrix<uint32_t> >();
                Matrix<uint32_t>& PlaneIdxSection1FlippedMatrix = *PlaneIdxSection1FlippedMatrixPtr;
                // flip section 1 idx
                PlaneIdxSection1FlippedMatrix = PlaneIdxSection1Matrix.flipped_lr();

                // -- Index information
                const uint32_t& n_largest = parameter.N_largest;

                IdxCorrespondenceMatrixPtr = make_shared< Matrix<uint32_t> >( n_largest , n_largest );
                
                // -- Processing
                // get number of indices in idx files
                uint32_t max_idx0 = PlaneIdxSection0MatrixPtr->Max();
                uint32_t max_idx1 = PlaneIdxSection1MatrixPtr->Max();

                PlaneIdxSection0Histogram.InitHistogram(-0.5, 1, max_idx0+ 1); // start value 0, bin size 1, number of bins max_idx0+ 1
                PlaneIdxSection1Histogram.InitHistogram(-0.5, 1, max_idx1+ 1); // start value 0, bin size 1, number of bins max_idx1+ 1

                // get idx histograms
                for (uint32_t l = 0; l < SectionPlaneIdxMatrixLines; ++l)
                {
                    for (uint32_t p = 0; p < SectionPlaneIdxMatrixPixels; ++p)
                    {
                        if(PlaneIdxSection0Matrix[l][p])
                            PlaneIdxSection0Histogram.addSample(PlaneIdxSection0Matrix[l][p]);

                        if (PlaneIdxSection1Matrix[l][p])
                            PlaneIdxSection1Histogram.addSample(PlaneIdxSection1Matrix[l][p]);
                    }
                }

                // sort -> get largest planes
                histogram0_frequencies_sort_idcs = sort_indices(histogram0_frequencies, "descend");
                histogram1_frequencies_sort_idcs = sort_indices(histogram1_frequencies, "descend");

                uint32_t n_sort_idcs0 = histogram0_frequencies_sort_idcs.size();
                uint32_t n_sort_idcs1 = histogram1_frequencies_sort_idcs.size();

                // evaluate ( get corresponding indices of largest planes )
                for (uint32_t i = 0; i < n_largest; ++i)
                {
                    if (i < n_sort_idcs0)
                    {
                        uint32_t cur_search_idx = histogram0_frequencies_sort_idcs[i];

                        for (uint32_t l = 0; cur_search_idx && l < SectionPlaneIdxMatrixLines; ++l) // search index not 0 (background)
                        {

                            for (uint32_t p = 0; p < SectionPlaneIdxMatrixPixels; ++p)
                            {
                                uint32_t cur_idx0 = PlaneIdxSection0Matrix[l][p];

                                if (cur_idx0 == cur_search_idx)
                                {
                                    // index at pixel position in second section
                                    uint32_t cur_idx1 = PlaneIdxSection1FlippedMatrix[l][p];

#if _DEBUG
                                    uint32_t cur_idx1_ = PlaneIdxSection1Matrix[l][p];
                                    uint32_t cur_idx1__ = PlaneIdxSection1Matrix[l][SectionPlaneIdxMatrixPixels - (p + 1)];
#endif

                                    bool found = false;
                                    uint32_t j = 0;

                                    // ... what is the index in the sorted vector
                                    // (sort index is the same as the segmentation index)
                                    for (; !found && j < n_sort_idcs1; ++j)
                                    {
                                        if (histogram1_frequencies_sort_idcs[j] == cur_idx1)
                                        {
                                            found = true;
                                        }
                                    }

                                    // ... when index is found increment correspondence matrix for
                                    //     current position of sort vectors
                                    if (found)
                                    {
                                        j = j - 1;
                                        IdxCorrespondenceMatrixPtr->SetVal(i, j, IdxCorrespondenceMatrixPtr->GetVal(i, j) + 1);
                                    }
                                }
                            }
                        }
                    }
                }

                // find n largest corresponding index entries ( ... decode sorting)

                //uint32_t n_same_idx = 0;

                Matrix<uint32_t> IdxCorrespondenceMatrixProcess(*IdxCorrespondenceMatrixPtr);

                const collected_plane_data_t& collected_plane_data_section_0= *plane_finder_processing_result_.collected_planes_sections_data[0];
                const collected_plane_data_t& collected_plane_data_section_1= *plane_finder_processing_result_.collected_planes_sections_data[1];

                const plane_data_point_ptr_vector_t& plane_data_points_section_0 = collected_plane_data_section_0.get_plane_data_points();
                const plane_data_point_ptr_vector_t& plane_data_points_section_1 = collected_plane_data_section_1.get_plane_data_points();

                double inv_idx_cnt_comparision_ratio_threshold= 1./ parameter.idx_cnt_comparision_ratio_threshold;

                for (uint32_t i = 0; i < n_largest; ++i)
                {
                    int row, col;

                    uint32_t common_index_cnt = IdxCorrespondenceMatrixProcess.Max(row, col);

                    if (common_index_cnt != 0)
                    {
                        // index count ratio
                        uint64_t current_plane_0_sort_idx = histogram0_frequencies_sort_idcs[row];
                        uint64_t current_plane_1_sort_idx = histogram1_frequencies_sort_idcs[col];

                        uint32_t current_plane_0_sort_idx_cnt = histogram0_frequencies[current_plane_0_sort_idx];
                        uint32_t current_plane_1_sort_idx_cnt = histogram1_frequencies[current_plane_1_sort_idx];

                        if (current_plane_0_sort_idx != 0 && current_plane_1_sort_idx != 0)
                        {

                            // == plane index counts comparison =============================================================
                            double ratio_of_index_cnts = current_plane_1_sort_idx_cnt / double(current_plane_0_sort_idx_cnt); // <= 1 A current_plane_0_sort_idx_cnt

                            bool idx_condition = ratio_of_index_cnts >= parameter.idx_cnt_comparision_ratio_threshold &&
                                ratio_of_index_cnts <= inv_idx_cnt_comparision_ratio_threshold;

                            if(!idx_condition)
                            {
                                IdxCorrespondenceMatrixProcess[row][col] = 0;
                                continue;
                            }

                            // == plane meta data evaluation =================================================================

                            plane_data_point_ptr_t cur_plane_0_data_point = plane_data_points_section_0[current_plane_0_sort_idx - 1];
                            plane_data_point_ptr_t cur_plane_1_data_point = plane_data_points_section_1[current_plane_1_sort_idx - 1];

                            // -- plane in desired region --------------------------------------------------------------------

                            double section0_plane_center_azimuth_angle = cur_plane_0_data_point->plane_center_azimuth.getAvr();
                            double section0_plane_center_elevation_angle = cur_plane_0_data_point->plane_center_elevation.getAvr();

                            bool plane_condition1 = parameter.scan_region_selection_vector_planes(section0_plane_center_azimuth_angle, section0_plane_center_elevation_angle).size() != 0;

                            if (!plane_condition1)
                            {
                                IdxCorrespondenceMatrixProcess[row][col] = 0;
                                continue;
                            }

                            // -- plane normals and center position ----------------------------------------------------------
                            vec    normal_plane_0, normal_plane_1;
                            vec    normal_std_plane_0, normal_std_plane_1;
                            vec    center_position_plane_0, center_position_plane_1;
                            vec    center_position_std_plane_0, center_position_std_plane_1;
                            double plane_distance_to_coordinate_zero_0, plane_distance_to_coordinate_zero_1;

                            switch (parameter.collected_plane_parameter_retrieval_type)
                            {
                                case plane_connector_parameter_t::collected_plane_parameter_retrieval_type_t::averaged:
                                {
                                    // -- Variant A --
                                    normal_plane_0 = cur_plane_0_data_point->normal.getAvr();
                                    normal_std_plane_0 = cur_plane_0_data_point->normal.getStd();
                                    center_position_plane_0 = cur_plane_0_data_point->center_position.getAvr();
                                    center_position_std_plane_0 = cur_plane_0_data_point->center_position.getStd();
                                    plane_distance_to_coordinate_zero_0 = cur_plane_0_data_point->plane_distance.plane_distance_to_coordinate_center;

                                    normal_plane_1 = negate(cur_plane_1_data_point->normal.getAvr());
                                    normal_std_plane_1 = cur_plane_1_data_point->normal.getStd();
                                    center_position_plane_1 = cur_plane_1_data_point->center_position.getAvr();
                                    center_position_std_plane_1 = cur_plane_1_data_point->center_position.getStd();
                                    plane_distance_to_coordinate_zero_1 = cur_plane_0_data_point->plane_distance.plane_distance_to_coordinate_center;
                                
                                    break;
                                }
                                case plane_connector_parameter_t::collected_plane_parameter_retrieval_type_t::fit:
                                {
                                    // -- Variant B --
                                    normal_plane_0 = cur_plane_0_data_point->plane_normal_from_fit;
                                    center_position_plane_0 = cur_plane_0_data_point->plane_center_from_fit;
                                    plane_distance_to_coordinate_zero_0 = cur_plane_0_data_point->plane_distance.plane_distance_to_coordinate_center;

                                    normal_plane_1 = cur_plane_1_data_point->plane_normal_from_fit;
                                    center_position_plane_1 = cur_plane_1_data_point->plane_center_from_fit;
                                    plane_distance_to_coordinate_zero_1 = cur_plane_0_data_point->plane_distance.plane_distance_to_coordinate_center;
                                
                                    break;
                                }
                            }

                            bool plane_condition2 = plane_comparison.plane_comparision_condition( parameter.plane_comparison_option,
                                                                                                 center_position_plane_0,
                                                                                                 center_position_plane_1,
                                                                                                 parameter.position_search_radius_center_position,
                                                                                                 normal_plane_0,
                                                                                                 normal_plane_1,
                                                                                                 parameter.angle_search_radius_normal);

                            if (!plane_condition2)
                            {
                                IdxCorrespondenceMatrixProcess[row][col] = 0;
                                continue;
                            }

                            // assign corresponding plane index vector
                            scan_section_correspondences.corresponding_plane_index_vector.push_back
                            (
                                corresponding_index_t(histogram0_frequencies_sort_idcs[row], histogram1_frequencies_sort_idcs[col])
                            );

                            // collect further data if desired
                            if (parameter.data_generation_options.generate_corresponding_planes_normal_difference_angles)
                            {
                                scan_section_correspondences.corresponding_plane_normals_vector.push_back(corresponding_planes_vectors_t(normal_plane_0, normal_plane_1));

                                scan_section_correspondences.corresponding_plane_normals_sigma_vector.push_back(corresponding_planes_vectors_t(normal_std_plane_0, normal_std_plane_1));

                                scan_section_correspondences.corresponding_plane_centerpositions_vector.push_back(corresponding_planes_vectors_t(center_position_plane_0, center_position_plane_1));

                                scan_section_correspondences.corresponding_plane_centerpositions_sigma_vector.push_back(corresponding_planes_vectors_t(center_position_std_plane_0, center_position_std_plane_1));

                                scan_section_correspondences.corresponding_plane_normal_difference_angle_vector.push_back(std::acos(dot(normal_plane_0, normal_plane_1)));

                                scan_section_correspondences.corresponding_plane_distance_to_coordinate_zero_vector.push_back
                                (
                                    corresponding_plane_distance_to_coordinate_zero_t( plane_distance_to_coordinate_zero_0,
                                                                                       plane_distance_to_coordinate_zero_1)
                                );

                                vec angles_l1 = { cur_plane_0_data_point->plane_center_azimuth.getAvr(), cur_plane_0_data_point->plane_center_elevation.getAvr() };
                                vec angles_l2 = { cur_plane_1_data_point->plane_center_azimuth.getAvr(), cur_plane_1_data_point->plane_center_elevation.getAvr() };

                                scan_section_correspondences.corresponding_plane_mean_angles_vector.push_back
                                (
                                    corresponding_plane_mean_angles_t(angles_l1, angles_l2)
                                );
                            }

                            // prepare next plane correspondence assignment
                            IdxCorrespondenceMatrixProcess[row][col] = 0;

                            //if (histogram0_frequencies_sort_idcs[row] == histogram1_frequencies_sort_idcs[col])
                            //    n_same_idx++;
                        }
                    }
                }

#if 0
                IdxCorrespondenceMatrixPtr.reset();
#endif

                // -- Verify and Debug Outputs
                if (parameter.data_generation_options.write_debug_output)
                {
                    IdxInfoMatrixPtr = make_shared< Matrix<uint32_t> >(5, n_largest);
                    Matrix<uint32_t>& IdxInfoMatrix = *IdxInfoMatrixPtr;

                    // debug output (for matlab)
                    for (uint32_t i = 0; i < n_largest; ++i)
                    {
                        if (i < n_sort_idcs0)
                        {
                            IdxInfoMatrix[0][i] = histogram0_frequencies_sort_idcs[i];
                            IdxInfoMatrix[2][i] = histogram0_frequencies[histogram0_frequencies_sort_idcs[i]];
                        }

                        if (i < n_sort_idcs1)
                        {
                            IdxInfoMatrix[1][i] = histogram1_frequencies_sort_idcs[i];
                            IdxInfoMatrix[3][i] = histogram1_frequencies[histogram1_frequencies_sort_idcs[i]];
                        }
                    }
                }

                if (parameter.data_generation_options.write_debug_output ||
                    parameter.data_generation_options.generate_largest_corresponding_planes_indicating_matrices)
                {
                    CorrespPlaneIdxSection0MatrixPtr = make_shared< Matrix<uint32_t> >(SectionPlaneIdxMatrixLines, SectionPlaneIdxMatrixPixels);
                    CorrespPlaneIdxSection1MatrixPtr = make_shared< Matrix<uint32_t> >(SectionPlaneIdxMatrixLines, SectionPlaneIdxMatrixPixels);

                    Matrix<uint32_t>& CorrespPlaneIdxSection0Matrix = *CorrespPlaneIdxSection0MatrixPtr;
                    Matrix<uint32_t>& CorrespPlaneIdxSection1Matrix = *CorrespPlaneIdxSection1MatrixPtr;

                    Matrix<uint32_t>& IdxInfoMatrix = *IdxInfoMatrixPtr;

                    // draw largest corresponding planes with the same index
                    uint32_t common_plane_idx = 0;

                    for (int i = 0; i < static_cast<int>(scan_section_correspondences.corresponding_plane_index_vector.size()); ++i)
                    {
                        const corresponding_index_t& cur_corresponding_index = scan_section_correspondences.corresponding_plane_index_vector[i];

                        ++common_plane_idx;

                        IdxInfoMatrix[4][i] = common_plane_idx;

                        for (uint32_t l = 0; l < SectionPlaneIdxMatrixLines; ++l)
                        {
                            for (uint32_t p = 0; p < SectionPlaneIdxMatrixPixels; ++p)
                            {
                                if (PlaneIdxSection0Matrix[l][p] == cur_corresponding_index.first)
                                    CorrespPlaneIdxSection0Matrix[l][p] = common_plane_idx;

                                if (PlaneIdxSection1Matrix[l][p] == cur_corresponding_index.second)
                                    CorrespPlaneIdxSection1Matrix[l][p] = common_plane_idx;

                            }
                        }
                    }
                }
                return true;
            }

            // ==================================================================================

            // corresponding planes are organized and found from sections (which devide the scan in contiguous 180° _azimuth_ parts)
            // this routine reorganizes the corespondences from sections to layers, which devide a (dual section) scan into _elevation_ parts [0 .. pi[ and [pi .. 2.pi[
            // 
            void reorganize_and_combine_from_sections_to_layers()
            {
            
                combine_sections_to_layer(0);
                combine_sections_to_layer(1);

            }

            // ==================================================================================
            
            using layer_plane_idx_vector_t = std::vector<uint32_t>;
            using layer_plane_section_vector_t = std::vector<uint32_t>;
            using layer_plane_nv_vector_t = std::vector<vec>;
            using layer_plane_d_vector_t = std::vector<vec>;

            void combine_sections_to_layer(uint32_t layer_idx)
            {
                uint32_t length_vectors = scan_section_correspondences.corresponding_plane_index_vector.size();

                layer_plane_idx_vector_t layer_plane_idx_vector(length_vectors);
                layer_plane_section_vector_t layer_plane_section_vector(length_vectors);

                for (uint32_t cor_idx = 0; cor_idx < length_vectors; cor_idx++)
                {
                    // check elevation < pi => section 0
                    if (scan_section_correspondences.corresponding_plane_mean_angles_vector[cor_idx].first[1] < PI) // sec 0 plane
                    {
                        layer_plane_section_vector[cor_idx] = 0;
                        layer_plane_idx_vector[cor_idx] = scan_section_correspondences.corresponding_plane_index_vector[cor_idx].first;
                    }
                    else // sec 1 plane
                    {
                        layer_plane_section_vector[cor_idx] = 1;
                        layer_plane_idx_vector[cor_idx] = scan_section_correspondences.corresponding_plane_index_vector[cor_idx].second;
                    }
                }

                for (uint32_t layer_plane_idx_ref = 0; layer_plane_idx_ref < length_vectors- 1; layer_plane_idx_ref++)
                {
                    vec normal_ref;
                    double d0_ref{ 0 };

                    if (layer_plane_section_vector[layer_plane_idx_ref] == 0)
                    {
                        d0_ref = scan_section_correspondences.corresponding_plane_distance_to_coordinate_zero_vector[layer_plane_idx_vector[layer_plane_idx_ref]].first;
                        normal_ref = scan_section_correspondences.corresponding_plane_normals_vector[layer_plane_idx_vector[layer_plane_idx_ref]].first;
                    }
                    else
                    {
                        d0_ref = scan_section_correspondences.corresponding_plane_distance_to_coordinate_zero_vector[layer_plane_idx_vector[layer_plane_idx_ref]].second;
                        normal_ref = scan_section_correspondences.corresponding_plane_normals_vector[layer_plane_idx_vector[layer_plane_idx_ref]].second;
                    }


                    for (uint32_t layer_plane_idx_search = layer_plane_idx_ref+ 1; layer_plane_idx_search < length_vectors; layer_plane_idx_search++)
                    {
                        
                    }
                }


            }

            // ==================================================================================

            bool write_debug_layers(const wstring& scan_file_path)
            {
                using namespace boost::filesystem;

                if (PlaneIdxSection0MatrixPtr && PlaneIdxSection1MatrixPtr)
                {

                    scan_data_layer_path_generator_t scan_data_layer_path_generator;

                    if (!scan_data_layer_path_generator.set(scan_file_path))
                        return false;

                    // generate layer file names

                    // files in section 0 directory
                    wstring section_0_layer_file_path_and_name_prefix = scan_data_layer_path_generator.get_data_layer_file_path_and_name_prefix(0);

                    wstring idx_file0 = section_0_layer_file_path_and_name_prefix + L".plane_idx_matrix.zfl";
                    wstring idx_file0_largest = section_0_layer_file_path_and_name_prefix + L".plane_idx_matrix.largest.zfl";

                    // files in section 1 directory
                    wstring section_1_layer_file_path_and_name_prefix = scan_data_layer_path_generator.get_data_layer_file_path_and_name_prefix(1);

                    wstring idx_file1 = section_1_layer_file_path_and_name_prefix + L".plane_idx_matrix.zfl";
                    wstring idx_file1_flipped = section_1_layer_file_path_and_name_prefix + L".plane_idx_matrix.flipped.zfl";
                    wstring idx_file1_largest = section_1_layer_file_path_and_name_prefix + L".plane_idx_matrix.largest.zfl";

                    // files in scan layer directory
                    wstring scan_name = scan_data_layer_path_generator.get_scan_name();
                    wstring scan_sections_data_layer_path = scan_data_layer_path_generator.get_scan_sections_data_layer_path();

                    wstring idx_correspondence = scan_sections_data_layer_path + L"/" + scan_name + L".idx_correspondence.zfl";
                    wstring idx_info = scan_sections_data_layer_path + L"/" + scan_name + L".idx_info.zfl";

                    wstring corresponding_plane_info_txt_file_name = scan_sections_data_layer_path + L"/" + scan_name + L".corresponding_plane_info.txt";

                    // processing
                    uint32_t n_largest = parameter.N_largest;

                    // plane idx section layer
                    if ((!exists(idx_file0) || !exists(idx_file1)))
                    {
                        ZFLayerI32 PlaneIdxSection0Layer, PlaneIdxSection1Layer;

                        if (!PlaneIdxSection0Layer.createAndOpen(SectionPlaneIdxMatrixPixels, SectionPlaneIdxMatrixLines, idx_file0))
                            return false;

                        if (!PlaneIdxSection1Layer.createAndOpen(SectionPlaneIdxMatrixPixels, SectionPlaneIdxMatrixLines, idx_file1))
                            return false;

                        Matrix<uint32_t>& PlaneIdxSection0Matrix = *PlaneIdxSection0MatrixPtr;
                        Matrix<uint32_t>& PlaneIdxSection1Matrix = *PlaneIdxSection1MatrixPtr;

                        for (uint32_t line = 0; line < SectionPlaneIdxMatrixLines; line++)
                        {
                            for (uint32_t pixel = 0; pixel < SectionPlaneIdxMatrixPixels; pixel++)
                            {
                                PlaneIdxSection0Layer.setVal(pixel, line, PlaneIdxSection0Matrix[line][pixel]);
                                PlaneIdxSection1Layer.setVal(pixel, line, PlaneIdxSection1Matrix[line][pixel]);
                            }
                        }
                    }

                    // plane idx section matrix
                    if (PlaneIdxSection1FlippedMatrixPtr)
                    {
                        ZFLayerI32 PlaneIdxSection1FlippedLayer;

                        if (!PlaneIdxSection1FlippedLayer.createAndOpen(SectionPlaneIdxMatrixPixels, SectionPlaneIdxMatrixLines, idx_file1_flipped))
                            return false;

                        Matrix<uint32_t>& PlaneIdxSection1FlippedMatrix = *PlaneIdxSection1FlippedMatrixPtr;

                        for (uint32_t line = 0; line < SectionPlaneIdxMatrixLines; line++)
                            for (uint32_t pixel = 0; pixel < SectionPlaneIdxMatrixPixels; pixel++)
                                PlaneIdxSection1FlippedLayer.setVal(pixel, line, PlaneIdxSection1FlippedMatrix[line][pixel]);
                    }

                    // corresponding plane idx section matrices
                    if (CorrespPlaneIdxSection0MatrixPtr && CorrespPlaneIdxSection1MatrixPtr)
                    {
                        ZFLayerI32 CorrespPlaneIdxSection0Layer, CorrespPlaneIdxSection1Layer;

                        if (!CorrespPlaneIdxSection0Layer.createAndOpen(SectionPlaneIdxMatrixPixels, SectionPlaneIdxMatrixLines, idx_file0_largest))
                            return false;

                        if (!CorrespPlaneIdxSection1Layer.createAndOpen(SectionPlaneIdxMatrixPixels, SectionPlaneIdxMatrixLines, idx_file1_largest))
                            return false;

                        Matrix<uint32_t>& CorrespPlaneIdxSection0Matrix = *CorrespPlaneIdxSection0MatrixPtr;
                        Matrix<uint32_t>& CorrespPlaneIdxSection1Matrix = *CorrespPlaneIdxSection1MatrixPtr;

                        // copy matrices to layers
                        for (uint32_t line = 0; line < SectionPlaneIdxMatrixLines; line++)
                        {
                            for (uint32_t pixel = 0; pixel < SectionPlaneIdxMatrixPixels; pixel++)
                            {
                                CorrespPlaneIdxSection0Layer.setVal(pixel, line, CorrespPlaneIdxSection0Matrix[line][pixel]);
                                CorrespPlaneIdxSection1Layer.setVal(pixel, line, CorrespPlaneIdxSection1Matrix[line][pixel]);
                            }
                        }
                    }

                    // Index Correspondence Information
                    if (IdxCorrespondenceMatrixPtr)
                    {
                        ZFLayerI32 IdxCorrespondenceLayer;

                        Matrix<uint32_t>& IdxCorrespondenceMatrix = *IdxCorrespondenceMatrixPtr;

                        if (!IdxCorrespondenceLayer.createAndOpen(n_largest, n_largest, idx_correspondence))
                            return false;

                        for (uint32_t line = 0; line < n_largest; line++)
                            for (uint32_t pixel = 0; pixel < n_largest; pixel++)
                                IdxCorrespondenceLayer.setVal(pixel, line, IdxCorrespondenceMatrix[line][pixel]);

                    }

                    if (IdxInfoMatrixPtr)
                    {
                        ZFLayerI32 IdxInfoLayer;

                        Matrix<uint32_t>& IdxInfoMatrix = *IdxInfoMatrixPtr;

                        if (!IdxInfoLayer.createAndOpen(n_largest, IdxInfoMatrix.Lines(), idx_info))
                            return false;

                        for (uint32_t line = 0; line < static_cast<uint32_t>(IdxInfoMatrix.Lines()); line++)
                        {
                            for (uint32_t pixel = 0; pixel < n_largest; pixel++)
                            {
                                IdxInfoLayer.setVal(pixel, line, IdxInfoMatrix[line][pixel]);
                            }
                        }
                    }

                    // Write Correspondences to .txt-file
                    if (parameter.data_generation_options.generate_corresponding_planes_normal_difference_angles)
                    {
                        zf::FileName fn(corresponding_plane_info_txt_file_name);
                        std::ofstream outfile;
                        outfile.open(fn.ToNative());

                        if (outfile.is_open())
                        {
                            outfile << " idx layer 1"<< " idx layer 2";

                            outfile << left;
                            outfile << setw(40);
                            outfile << " normal layer 1";
                            outfile << left;
                            outfile << setw(40);
                            outfile << " normal layer 2";

                            outfile << "   difference angle";

                            outfile << left;
                            outfile << setw(19);
                            outfile << " plane center angles layer 1";

                            outfile << left;
                            outfile << setw(19);
                            outfile << " plane center angles layer 2";

                            outfile << "   plane distance 1";
                            outfile << "   plane distance 2";

                            //outfile << left;
                            //outfile << setw(34);
                            //outfile << " normal sigma layer 1";
                            //outfile << left;
                            //outfile << setw(34);
                            //outfile << " normal sigma layer 2";

                            outfile << left;
                            outfile << setw(34);
                            outfile << " center positions layer 1";
                            outfile << left;
                            outfile << setw(34);
                            outfile << " center positions layer 2";

                            //outfile << left;
                            //outfile << setw(34);
                            //outfile << " center positions sigma layer 1";
                            //outfile << left;
                            //outfile << setw(34);
                            //outfile << " center positions sigma layer 2";

                            outfile << "\n\n";

                            for (size_t n_plane = 0; n_plane < scan_section_correspondences.corresponding_plane_index_vector.size(); n_plane++)
                            {
                                outfile << right;
                                outfile << setw(12);
                                outfile << scan_section_correspondences.corresponding_plane_index_vector[n_plane].first;
                                outfile << right;
                                outfile << setw(12);
                                outfile << scan_section_correspondences.corresponding_plane_index_vector[n_plane].second;
                                
                                outfile << setprecision(8); 
                                outfile << fixed;
                                outfile << right;
                                outfile << showpos;

                                outfile << " (" <<
                                    scan_section_correspondences.corresponding_plane_normals_vector[n_plane].first[0] << ", " <<
                                    scan_section_correspondences.corresponding_plane_normals_vector[n_plane].first[1] << ", " <<
                                    scan_section_correspondences.corresponding_plane_normals_vector[n_plane].first[2] << ") (" <<
                                    scan_section_correspondences.corresponding_plane_normals_vector[n_plane].second[0] << ", " <<
                                    scan_section_correspondences.corresponding_plane_normals_vector[n_plane].second[1] << ", " <<
                                    scan_section_correspondences.corresponding_plane_normals_vector[n_plane].second[2] << ")";

                                outfile << setw(19);
                                outfile << fixed;
                                outfile << right;

                                outfile << scan_section_correspondences.corresponding_plane_normal_difference_angle_vector[n_plane];

                                outfile << setprecision(6);
                                outfile << fixed;
                                outfile << right;
                                outfile << showpos;

                                outfile << "      (" <<
                                    scan_section_correspondences.corresponding_plane_mean_angles_vector[n_plane].first[0] << ", " <<
                                    scan_section_correspondences.corresponding_plane_mean_angles_vector[n_plane].first[1] << ")";

                                outfile << "      (" <<
                                    scan_section_correspondences.corresponding_plane_mean_angles_vector[n_plane].second[0] << ", " <<
                                    scan_section_correspondences.corresponding_plane_mean_angles_vector[n_plane].second[1] << ")";

                                outfile << setw(19);
                                outfile << fixed;
                                outfile << right;

                                outfile << scan_section_correspondences.corresponding_plane_distance_to_coordinate_zero_vector[n_plane].first;

                                outfile << setw(19);
                                outfile << fixed;
                                outfile << right;

                                outfile << scan_section_correspondences.corresponding_plane_distance_to_coordinate_zero_vector[n_plane].second;

                                //outfile << setprecision(1);
                                //outfile << scientific;
                                //
                                //outfile << " (" <<
                                //    corresponding_plane_normals_sigma_vector[n_plane].first[0] << ", " <<
                                //    corresponding_plane_normals_sigma_vector[n_plane].first[1] << ", " <<
                                //    corresponding_plane_normals_sigma_vector[n_plane].first[2] << ") (" <<
                                //    corresponding_plane_normals_sigma_vector[n_plane].second[0] << ", " <<
                                //    corresponding_plane_normals_sigma_vector[n_plane].second[1] << ", " <<
                                //    corresponding_plane_normals_sigma_vector[n_plane].second[2] << ")";

                                outfile << setprecision(6);
                                outfile << fixed;

                                outfile << " (" <<
                                    scan_section_correspondences.corresponding_plane_centerpositions_vector[n_plane].first[0] << ", " <<
                                    scan_section_correspondences.corresponding_plane_centerpositions_vector[n_plane].first[1] << ", " <<
                                    scan_section_correspondences.corresponding_plane_centerpositions_vector[n_plane].first[2] << ") (" <<
                                    scan_section_correspondences.corresponding_plane_centerpositions_vector[n_plane].second[0] << ", " <<
                                    scan_section_correspondences.corresponding_plane_centerpositions_vector[n_plane].second[1] << ", " <<
                                    scan_section_correspondences.corresponding_plane_centerpositions_vector[n_plane].second[2] << ")";

                                //outfile << setprecision(1);
                                //outfile << scientific;
                                //
                                //outfile << " (" <<
                                //    corresponding_plane_centerpositions_sigma_vector[n_plane].first[0] << ", " <<
                                //    corresponding_plane_centerpositions_sigma_vector[n_plane].first[1] << ", " <<
                                //    corresponding_plane_centerpositions_sigma_vector[n_plane].first[2] << ") (" <<
                                //    corresponding_plane_centerpositions_sigma_vector[n_plane].second[0] << ", " <<
                                //    corresponding_plane_centerpositions_sigma_vector[n_plane].second[1] << ", " <<
                                //    corresponding_plane_centerpositions_sigma_vector[n_plane].second[2] << ")";

                                outfile << "\n";

                            }
                            outfile.close();
                        }
                    }
                    return true;
                    // on return layers are written to disk
                }
                else
                {
                    return false;
                }
                
            }

            // ==================================================================================

        };
    }
}
#endif