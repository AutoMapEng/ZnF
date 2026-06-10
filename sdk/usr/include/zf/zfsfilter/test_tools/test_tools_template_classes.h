#pragma once

#include <zfsfilter/Matrix.h>

#define MAX_ROWS 1000

namespace zfsfilter
{
    namespace test
    {
        template<class T> class HelperMatrixClass
        {
            public:
                HelperMatrixClass( const size_t aSize )
                {
                    init( aSize, aSize );
                }
            
                HelperMatrixClass( const size_t aLines, const size_t aColumns )
                {
                    init( aLines, aColumns );
                }
            
                HelperMatrixClass( const zfsfilter::Matrix<T>& aMatrix )
                {
                    if(aMatrix.Lines() && aMatrix.Columns())
                    {
                        init( aMatrix.Lines(), aMatrix.Columns() );
            
                        for(int m = 0; m < aMatrix.Lines(); ++m)
                            for(int n = 0; n < aMatrix.Columns(); ++n)
                                VectorOfVectors[m][n] = aMatrix[m][n];
            
                    }
            
                }
            
                void init( const size_t aLines, const size_t aColumns )
                {
                    VectorOfVectors = std::vector< std::vector< T > >( aLines, std::vector< T >( aColumns, 0 ) );
                }
            
                size_t Lines() const
                {
                    return VectorOfVectors.size();
                }
            
                size_t Columns() const
                {
                    return VectorOfVectors[0].size();
                }
            
            protected:
            
                std::vector< std::vector< T > > VectorOfVectors;
        
        
        };
    }
}