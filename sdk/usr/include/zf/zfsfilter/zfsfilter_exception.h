#pragma once

/**
 * @brief Definition of Exception Class
 *
 * This file contains definitions for a basic exception class
 * for zfsfilter
 *
 * @author  Jürgen Holzner (Zoller + Fröhlich GmbH)
 * @date    $Date: 2015-06-19 08:00:00 +0200 (Fr, 19 Jun 2015) $
 * @version $Revision: 0 $
 */

#include "filterapi.h"
#include <zfs/Math/auxilliary/exception.h>

namespace zfsfilter
{
    //========================================================================================================================================================
    struct ZFSFilterException : public zfs::math::ZFSMathException
    {
        ZFSFilterException(std::string a_ExceptionString) : ZFSMathException(a_ExceptionString) {}
        ~ZFSFilterException() NOEXCEPT {}
    };
    //========================================================================================================================================================
    // Example Specialization
    //
    //struct FilterException: public ZFSFilterException
    //{
    //  FilterException(std::string a_ExceptionString): ZFSFilterException(a_ExceptionString){}
    //  ~FilterException() NOEXCEPT {}
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
} // zfsfilter
