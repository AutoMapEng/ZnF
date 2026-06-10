#pragma once

/**
 * \file range.h
 * @brief Definition of Range Class
 *
 * This file contains all definitions for the Range class
 * including specialisations for integer types
 *
 * @author  Jürgen Holzner (Zoller + Fröhlich GmbH)
 * @date    $Date: 2014-08-08 12:59:59 +0200 (Fr, 08 Aug 2014) $
 * @version $Revision: 80 $
 */

#include <zfs/zfsapi.h>

#include <cstdint>
#include <string>

#include <zfs/Math/auxilliary/exception.h>
#include "tuple.h"

namespace zfs {
    namespace math {
        //========================================================================================================================================================
        // Exception class for tuple (maybe specialization necessary)
        struct RangeException : public ZFSMathException
        {
            RangeException(const std::string& a_ExceptionString) : ZFSMathException(a_ExceptionString) {}
            ~RangeException() NOEXCEPT {}
        };
        //========================================================================================================================================================

        /** \class Range
         * \brief  This class represents a closed interval for types that can be ordered.
         *
         * \author Juergen Holzner
         *
         * This class represents a closed interval for types that can be ordered.
         *
         */

        template<class T>
        class ZFS_API Range : public Tuple2Base<T>
        {
        protected:
            T m_Length;

        public:
            // constructors
            Range();
            Range(const Range<T>& a_range);
            Range(const T& a_first, const T& a_second);
            virtual ~Range();

            const bool isEmptyRange() const;
            const bool isInRange(const T& value) const;

            T lowerborder() const;
            T upperborder() const;

            // getter functions
            const T Length() const;

            /// Shift the range to the right.
            const Range<T> operator+(const T& rhs) const;
            /// Shift the range to the left.
            const Range<T> operator-(const T& rhs) const;

            Range<T>& operator=(const Range<T>& a_range);

            /// Return a range with values that are in this AND other.
            const Range<T> intersection(const Range<T>& other) const;

        private:
            void enforceClassConstraint();

        };
        //========================================================================================================================================================

    } // end namespace math
} // end namespace zfs