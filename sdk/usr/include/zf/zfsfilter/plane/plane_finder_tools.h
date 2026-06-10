#pragma once

#include <zfsfilter/filterapi.h>

#include <vector>

namespace zfsfilter
{
    namespace planes
    {
        // ========================================================================================================================
        using vec = std::vector<double>;
        // ========================================================================================================================
        FILTERDLL_API double norm(const vec& a);
        FILTERDLL_API vec cross(const vec& a, const vec& b);
        FILTERDLL_API void generate_plane_vectors_from_normal(const vec& normal, vec& plane_vector_1, vec& plane_vector_2);
        // ========================================================================================================================
    }
}