#pragma once

/**
*	Stack of matrices
*/
#include "filterapi.h"

#include <string>
#include <vector>

#include "Matrix.h"

namespace zfsfilter
{

    template<class T> class FILTERDLL_API Matrixstack
    {

    public:
        Matrixstack( const size_t a_NoOfLayers, const int a_M= 0, const int a_N= 0 )
        { 
            setNoOfLayers( a_NoOfLayers, a_M, a_N );
        }

        Matrixstack( const std::vector<uint8_t>& a_LayerIdx, const int a_M= 0, const int a_N= 0 )
        { 
            setLayers( a_LayerIdx, a_M, a_N );
        }

        Matrix<T>& operator[]( const uint8_t a_LayerIdx );       /// < setter
        Matrix<T>  operator[]( const uint8_t a_LayerIdx ) const; /// < getter

        size_t NoOfLayers() const { return m_Stack.size();}
        void   setNoOfLayers( const size_t a_NoOfLayers, const int a_M= 0, const int a_N= 0 );
        void   setLayers( std::vector<uint8_t> a_LayerAdressingVector, const int a_M= 0, const int a_N= 0 );

        void empty();

    protected:
        std::vector< Matrix<T> > m_Stack;
        std::vector< uint8_t >   m_LayerAdressing; /// < layer mapping capability, number of layers restricted by uint8_t to 256
    };

}