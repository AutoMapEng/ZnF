#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>
#include <utility>
#include <vector>
#include <memory>

#include <zfsfilter/kdtree.h>

#include "plane_data_point.h"

namespace zfsfilter
{
    namespace planes
    {
        // ========================================================================================================================
        using namespace kdt;
        // ========================================================================================================================
        using pt_t = std::pair<uint32_t, uint32_t>;
        using pt_vector_t = std::vector<pt_t>;

        using vec_ptr_t = shared_ptr<const std::vector<double>>; // ptr to plane center
        using vec_vec_ptr_t = std::vector< vec_ptr_t >; // vector to of ptrs to plane center
        // ========================================================================================================================
        typedef std::shared_ptr<plane_data_point_t> plane_data_pt_ptr_t;
        // ========================================================================================================================
        class kdtree_plane_pt_t
        {
        public:

            // dimension of space (or "k" of k-d tree)
            // KDTree class accesses this member
            enum { DIM = 3 };

            plane_data_pt_ptr_t pt_ptr;

            kdtree_plane_pt_t(){}
            kdtree_plane_pt_t(plane_data_pt_ptr_t v) : pt_ptr(v) {}

            double operator[](uint32_t idx) const;
        };
        // ========================================================================================================================
        inline double kdtree_plane_pt_t::operator[](uint32_t idx) const
        {
            switch (pt_ptr->collected_plane_parameter_retrieval_type)
            {

            case plane_connector_parameter_t::collected_plane_parameter_retrieval_type_t::fit:
            {
                return pt_ptr->plane_center_from_fit[idx];
            }

            case plane_connector_parameter_t::collected_plane_parameter_retrieval_type_t::averaged:
            {
                return pt_ptr->center_position.getAvr()[idx];
            }

            default:
            {
                return pt_ptr->plane_center_from_fit[idx];
            }

            }

            return -1;
        }
        // ========================================================================================================================
        typedef KDTree<kdtree_plane_pt_t> plane_kdtree_t;
        // ========================================================================================================================
    }
}
