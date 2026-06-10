#pragma once

#include <list>
#include <vector>
#include <cstdint>

#include <zfs/zfsapi.h>

typedef unsigned char byte;

namespace zfs {
    namespace math {

        // =====================================================================================================================================================================================
        enum class list_condition_indicator :uint8_t { LIST_OP_GT, LIST_OP_GE, LIST_OP_LT, LIST_OP_LE, LIST_OP_EQ };
        // =====================================================================================================================================================================================
        template<class T> T                ZFS_API median(const std::list<T>& list);
        template<class T> T                ZFS_API max_list(const std::list<T>& list);
        template<class T> T                ZFS_API min_list(const std::list<T>& list);
        template<class T> T                ZFS_API sum_elements(const std::list<T>& list);
        // =====================================================================================================================================================================================
        template<class T> int              ZFS_API count_elements_meeting_condition(const std::list<T>& a_list, const T& a_threshold, const list_condition_indicator& a_cond_ind);
        template<class T> std::vector<int> ZFS_API histogram(const std::list<T>& data_list, T bin_size, T hist_start, T hist_end);
        // =====================================================================================================================================================================================
        template<class T> void             ZFS_API limit_values_upper_bound(std::list<T>& data_list, const T& max_value);
        template<class T> void             ZFS_API limit_values_lower_bound(std::list<T>& data_list, const T& min_value);
        template<class T> void             ZFS_API limit_values(std::list<T>& data_list, const T& min_value, const T& max_value);
        // =====================================================================================================================================================================================
        template<class T> bool             ZFS_API list_equal(const std::list<T>& lhs, const std::list<T>& rhs);
        // =====================================================================================================================================================================================
#define LIST_OPERATOR_INSTANCIATIONS(OPERATOR_TYPE)\
template <typename T> ZFS_API std::list<T> OPERATOR_TYPE(const std::list<T>& a, const std::list<T>& b);\
template <typename T> ZFS_API std::list<T> OPERATOR_TYPE(const std::list<T>& a, const T& value);

            LIST_OPERATOR_INSTANCIATIONS(operator+)
            LIST_OPERATOR_INSTANCIATIONS(operator-)
            LIST_OPERATOR_INSTANCIATIONS(operator*)
            LIST_OPERATOR_INSTANCIATIONS(operator/)

#undef LIST_OPERATOR_INSTANCIATIONS
            // =====================================================================================================================================================================================

    } // end namespace math
} // end namespace zfs