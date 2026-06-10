#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>

#include <zfs/zfsapi.h>

/*this are helping functions for handling stl vector class*/

typedef unsigned char byte;

namespace zfs {
    namespace math {
        // =====================================================================================================================================================================================
        enum class vector_comparison_t { VECTOR_OP_GT, VECTOR_OP_GE, VECTOR_OP_LT, VECTOR_OP_LE, VECTOR_OP_EQ };
        // =====================================================================================================================================================================================
        template <class T> class Range;
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<T> stl_subvector(const std::vector<T>& aVector, const Range<int>& aRange);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<T> stl_subvector(const std::vector<T>& aVector, const int aStart, const int aEnd);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<T> reverse(const std::vector<T>& aVector);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<T> abs(const std::vector<T>& a);
        template <typename T> ZFS_API std::vector<T> abs(const T* a, int length);
        template <typename T> ZFS_API std::vector<T> negate(const std::vector<T>& a);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<size_t> NonZero(const std::vector<T>& aValueVector);
        template <typename T> ZFS_API std::vector<size_t> WithinRange(const std::vector<T>& aValueVector, const T& aMin, const T& aMax);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<T> GetValues(const std::vector<T>& aValueVector, const std::vector<bool>& aIndexVector);
        template <typename T> ZFS_API std::vector<T> GetValues(const std::vector<T>& aValueVector, const std::vector<int>& aIndexVector);
        template <typename T> ZFS_API std::vector<T> GetValues(const std::vector<T>& aValueVector, const std::vector<size_t>& aIndexVector);
        template <typename T1, typename T2> ZFS_API void GetValues(const std::vector<T1>& aValueVector, const std::vector<size_t>& aIndexVector, std::vector<T2>& aOutputVector);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b);
        template <typename T> ZFS_API std::vector<T> operator+(const std::vector<T>& a, T value);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<T> operator*(const std::vector<T>& a, const std::vector<T>& b);
        template <typename T> ZFS_API std::vector<T> operator*(const std::vector<T>& a, T value);
        template <typename T> ZFS_API std::vector<T> operator*(T value, const std::vector<T>& a);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<T> operator/(const std::vector<T>& a, const std::vector<T>& b);
        template <typename T> ZFS_API std::vector<T> operator/(const std::vector<T>& a, T value);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<T> operator-(const std::vector<T>& a, const std::vector<T>& b);
        template <typename T> ZFS_API std::vector<T> operator-(const std::vector<T>& a, T value);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<T> sqrt(const std::vector<T>& a);
        template <typename T> ZFS_API std::vector<T> sqr(const std::vector<T>& a);
        template <typename T> ZFS_API std::vector<T> ln(const std::vector<T>& a);
        template <typename T> ZFS_API std::vector<T> log10(const std::vector<T>& a);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API T dot(const std::vector<T>& a, const std::vector<T>& b);
        template <typename T> ZFS_API double length(const std::vector<T>& a);
        template <typename T> ZFS_API double angle(const std::vector<T>& a, const std::vector<T>& b);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<T> normalize_1(const std::vector<T>& a); // normalization by sum of vector values
        template <typename T> ZFS_API std::vector<T> normalize_2(const std::vector<T>& a); // normalization by square root os sum of squares of vector values
        template <typename T> ZFS_API std::vector<T> normalize_2(const T* a, int length); // normalization by square root os sum of squares of vector values
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<T> reciprocal0(const std::vector<T>& a); // if an element in a occurs == 0 output element is set to 0
        template <typename T> ZFS_API std::vector<T> reciprocal0thresh(const std::vector<T>& a, const T& eps); // if an element in a occurs < eps output element is set to 0
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<bool> operator> (const std::vector<T>& a, T value);
        template <typename T> ZFS_API std::vector<bool> operator>=(const std::vector<T>& a, T value);
        template <typename T> ZFS_API std::vector<bool> operator> (T value, const std::vector<T>& a);
        template <typename T> ZFS_API std::vector<bool> operator>=(T value, const std::vector<T>& a);
        template <typename T> ZFS_API std::vector<bool> operator> (const std::vector<T>& a, const std::vector<T>& b);
        template <typename T> ZFS_API std::vector<bool> operator>=(const std::vector<T>& a, const std::vector<T>& b);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<bool> operator< (const std::vector<T>& a, T value);
        template <typename T> ZFS_API std::vector<bool> operator<=(const std::vector<T>& a, T value);
        template <typename T> ZFS_API std::vector<bool> operator< (T value, const std::vector<T>& a);
        template <typename T> ZFS_API std::vector<bool> operator<=(T value, const std::vector<T>& a);
        template <typename T> ZFS_API std::vector<bool> operator< (const std::vector<T>& a, const std::vector<T>& b);
        template <typename T> ZFS_API std::vector<bool> operator<=(const std::vector<T>& a, const std::vector<T>& b);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<bool> operator==(const std::vector<T>& a, T value);
        template <typename T> ZFS_API std::vector<bool> operator==(T value, const std::vector<T>& a);
        template <typename T> ZFS_API std::vector<bool> operator==(const std::vector<T>& a, const std::vector<T>& b);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<T> max(const std::vector<T>& a, const std::vector<T>& b);
        template <typename T> ZFS_API std::vector<T> min(const std::vector<T>& a, const std::vector<T>& b);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API bool vector_equal(const std::vector<T>& lhs, const std::vector<T>& rhs, const T& TOL);
        // =====================================================================================================================================================================================
        template<class T> void ZFS_API limit_values_upper_bound(std::vector<T>& data_list, const T& max_value);
        template<class T> void ZFS_API limit_values_lower_bound(std::vector<T>& data_list, const T& min_value);
        template<class T> void ZFS_API limit_values(std::vector<T>& data_list, const T& min_value, const T& max_value);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API bool Any(const std::vector<T>& aVector);
        template <typename T> ZFS_API bool All(const std::vector<T>& aVector);
        // =====================================================================================================================================================================================
        ZFS_API bool Any(const std::vector<bool>& aVector);
        ZFS_API bool All(const std::vector<bool>& aVector);
        ZFS_API int  nElementsTrue(const std::vector<bool>& aVector);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API bool Any(const std::vector<T>& aVector, const T& aValue);
        template <typename T> ZFS_API bool All(const std::vector<T>& aVector, const T& aValue);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API bool AnyEQ(const std::vector<T>& aVector, const T& aValue, int& Position); /// < comparision by equal, not using abs(difference) < XXX_EPSILON
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API bool Any(const std::vector<T>& aVector, const T& aValue, const vector_comparison_t aVecOperator);
        template <typename T> ZFS_API bool All(const std::vector<T>& aVector, const T& aValue, const vector_comparison_t aVecOperator);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<T>    GenerateRandomVector(size_t L);
        ZFS_API std::vector<bool> GenerateRandomBoolVector(size_t L);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API int NElements(const std::vector<T>& aVector, const T& aValue);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API bool copy_range(const std::vector<T>& src, const Range<int>& src_rg, const Range<int>& dest_rg, std::vector<T>& dest);
        template <typename T> ZFS_API bool set_value_range(const T& a_value, const Range<int>& a_rg, std::vector<T>& v_dest);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API T        sum(const std::vector<T>& a);
        template <typename T> ZFS_API uint64_t sum_large_int(const std::vector<T>& a);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<int> sort_indices(const std::vector<T>& v, const std::string& sort_direction = "ascend");
        // sorting with seperately provided index vector:
        template <typename T> ZFS_API std::vector<int> sort_indices(const std::vector<T>& v, std::vector<int> idx_vector, const std::string& sort_direction = "ascend");
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API std::vector<int> stable_sort_indices(const std::vector<T>& v, const std::string& sort_direction = "ascend");
        template <typename T> ZFS_API std::vector<int> stable_sort_indices(const T* v, int length, const std::string& sort_direction = "ascend");
        // sorting with seperately provided index vector:
        template <typename T> ZFS_API std::vector<int> stable_sort_indices(const std::vector<T>& v, std::vector<int> idx_vector, const std::string& sort_direction = "ascend");
        // =====================================================================================================================================================================================
        // Pointer Versions
        // =====================================================================================================================================================================================
        // for int types (bool, int8_t, ..., uint8_t, ...)
        template <typename T> ZFS_API bool Any(T Vector[], const int aLength); // checks for !0
        template <typename T> ZFS_API bool All(T Vector[], const int aLength); // checks for !0
        template <typename T> ZFS_API bool Any(const T Vector[], const int aLength); // checks for !0
        template <typename T> ZFS_API bool All(const T Vector[], const int aLength); // checks for !0
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API bool Any(T Vector[], const int aLength, const T& aValue);
        template <typename T> ZFS_API bool All(T Vector[], const int aLength, const T& aValue);
        template <typename T> ZFS_API bool Any(const T Vector[], const int aLength, const T& aValue);
        template <typename T> ZFS_API bool All(const T Vector[], const int aLength, const T& aValue);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API int NElements(T Vector[], const int aLength, const T& aValue);
        template <typename T> ZFS_API int NElements(const T Vector[], const int aLength, const T& aValue);
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API void VecMinus(T Vector1[], T Vector2[], T ResultVector[], const int aLength); // aVector1- aVector2
        template <typename T> ZFS_API void VecMinus(const T Vector1[], const T Vector2[], T ResultVector[], const int aLength); // aVector1- aVector2
        // =====================================================================================================================================================================================
        template <typename T> ZFS_API void Copy(T* pSrcStart, const T* const pSrcEnd, T* pDestStart);
        //=======================================================================================
        template <typename T> ZFS_API int count_elements_meeting_condition(const std::vector<T>& a_list, const T& a_threshold, const vector_comparison_t& a_cond_ind);
        // =====================================================================================================================================================================================
    } // end namespace math
} // end namespace zfs