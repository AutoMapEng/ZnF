#pragma once

#include "filterapi.h"

#include <complex>
#include <vector>
#include <string>

typedef unsigned char byte;

namespace zfs
{
    namespace math
    {
        template<class T> class Point3D;
    }
}

namespace zfsfilter
{
    //================================================================================================================
    // types
    // vector: <T1>: {float, double}
    template<class T1> bool FILTERDLL_API print_complex_vector_to_file( const std::wstring& filename, const std::vector<std::complex<T1> >& vector );
    //================================================================================================================
    // types
    // vector: <T1>: all integral types, i.e. {bool, byte (= unsigned char), short, int, unsigned short, unsigned int, float, double, long double}
    template<class T1> bool FILTERDLL_API print_vector_to_file( const std::wstring& filename, const std::vector<T1>& vector );
    template<class T1> bool FILTERDLL_API print_vector_to_file( const std::wstring& filename, const std::vector<zfs::math::Point3D<T1>>& vector );
    //================================================================================================================
    // types
    // vector_x, vector_y, vector_z: <T1>: {float, double}
    // vector_i                          : <T2>: {int,float,double}
    template<class T1, class T2> bool FILTERDLL_API print_xyzi_vectors_to_file( const std::wstring& filename,
                                                                                const std::vector<T1>& vector_x,
                                                                                const std::vector<T1>& vector_y,
                                                                                const std::vector<T1>& vector_z,
                                                                                const std::vector<T2>& vector_i );
    //================================================================================================================
}//namespace zfsfilter
