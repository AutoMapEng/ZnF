#pragma once

#include <type_traits>
#include <typeinfo>
#include <cstdint>
#include <cstddef>
#include <string>

#include <zfs/Math/auxilliary/exception.h>

// ================================================================================
// cast to underlying type of an enum class:
// get the number representation/equivalent of a 
// particular enumerator of a enumeration type/class
// ================================================================================

template<typename E>
constexpr typename std::underlying_type<E>::type toUType(E enumerator) NOEXCEPT
{
    return static_cast<typename std::underlying_type<E>::type>(enumerator);
}


namespace zfs {
    namespace math {

        // ================================================================================

        template<typename T> bool IsFloat(const T& aVar)
        {

            std::string Varname = typeid(aVar).name();

            if (Varname == "float" || Varname == "double" || Varname == "long double") return true;
            else return false;

        }

        // ================================================================================

        template<typename T> bool IsInt(const T& aVar)
        {

            return !IsFloat(aVar);

        }

        // ================================================================================

    } // end namespace math
} // end namespace zfs