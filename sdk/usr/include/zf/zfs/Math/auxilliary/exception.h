#pragma once

#include <zfs/zfsapi.h>

#include <string>
#include <exception>

#if !defined(_MSC_VER) || (defined(_MSC_VER) && _MSC_VER >= 1900)
#define NOEXCEPT noexcept
#else
#define NOEXCEPT throw()
#endif


namespace zfs {
    namespace math {
        //========================================================================================================================================================
        /** \class ZFSMathException
         * \brief  Definition of a basic exception class for objects in zfs/Math.
         *
         * \author Juergen Holzner
         *
         * Definition of a basic exception class for objects in zfs/Math.
         *
         */

        struct ZFSMathException : public std::exception
        {
            std::string m_ExceptionString; // < captures exception description
            ZFSMathException(const std::string& a_ExceptionString) : m_ExceptionString(a_ExceptionString) {}
            ~ZFSMathException() NOEXCEPT {}
            const char* what() const NOEXCEPT { return m_ExceptionString.c_str(); }
        };
        //========================================================================================================================================================
        // Example Specialization
        //
        //struct RangeException: public ZFSMathException
        //{
        //  RangeException(std::string a_ExceptionString): ZFSMathException(a_ExceptionString){}
        //  ~RangeException() NOEXCEPT {}
        //};

        //========================================================================================================================================================
        // Example Application:
        //
        // try
        // {
        //     Tuple3<int> MyTuple(1,2,3);
        //     cout<<Tuple3[4]<<endl; // throws
        // }
        // catch( TupleException& exception_caugth)
        // {
        //     cout<< exception_caugth.what() << endl;
        // }
        //========================================================================================================================================================
    } // end namespace math
} // end namespace zfs