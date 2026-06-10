#pragma once

#if 1

#include <cstdint>

#include <vector>

#include "filterapi.h"

#include <zfs/Math/math/tuple.h>
#include <zfs/Math/signal_processing/window.h>

#include "Matrix.h"
#include "filtertable.h"

namespace zfsfilter
{

    /** @ class Gradient
     *  @ class for calculating the gradient based on convolution
     */
    template <typename data_t, typename grad_t> class FILTERDLL_API Gradient
    {
    public:

        /*gradient of a data matrix */
        Gradient( int aAveraging /*number of resolution cells to be averaged*/, 
                  data_t aResolution= 1. /*in pixels, if unknown set to 1*/);

        virtual ~Gradient();

        const Matrix<grad_t> &                              getGradX() const;          // (fx) x <==> gradient across column index
        const Matrix<grad_t> &                              getGradY() const;          // (fy) y <==> gradient across row index

        const Matrix<grad_t>                                getGradientLengthsSqr() const; // l= |fx|^2+|fy|^2
        const Matrix<grad_t>                                getGradientLengths()    const; // l= sqr(|fx|^2+|fy|^2)

        const Matrix<grad_t>                                getGradientLengthsL1()  const; // l'=|fx|+|fy|

        const Matrix<grad_t>                                getGradientAngles()     const; // alpha = atan2(fy,fx);

        void   setData( const Matrix< data_t >* aData );

    protected:

        // private variables
        Matrix<grad_t>                                    m_GradX, m_GradY;

        const Matrix<data_t>*                             m_DataAccess;

        const zfs::math::SignalWindowNameType                m_WindowName;

        const double                                      m_KernelParameter;
        const double                                      m_KernelLengthParameter;

        const uint16_t                                    m_Averaging;

        double                                            m_Resolution;

        // functions for calculating the gradients
        void gradX( const Matrix<data_t>& aData, Matrix<grad_t>& aGradX );
        void gradY( const Matrix<data_t>& aData, Matrix<grad_t>& aGradY );

        void calculate_gradients();

        double res2sigma() const;

    };

    /** @ class ScanDataGradient
     *  @ class for calculating the gradient based on convolution
     */
    template <typename data_t, typename grad_t, typename mpw_t> class FILTERDLL_API ScanDataGradient: public Gradient<data_t, grad_t>
    {
    public:

        /*gradient of a data matrix */
        ScanDataGradient( int aAveraging );

        void setData( const Matrix< data_t >* aData, const std::pair< const Matrix<mpw_t>*, const Matrix<mpw_t>* >& aMPW );

        const Matrix<grad_t> &                              getGradX() const;          // (fx)
        const Matrix<grad_t> &                              getGradY() const;          // (fy)

        const Matrix<grad_t>                                getGradientLengthsSqr() const; // l= |fx|^2+|fy|^2
        const Matrix<grad_t>                                getGradientLengths()    const; // l= sqr(|fx|^2+|fy|^2)

        const Matrix<grad_t>                                getGradientLengthsL1()  const; // l'=|fx|+|fy|

        const Matrix<grad_t>                                getGradientAngles()     const; // alpha = atan2(fy,fx);

    protected:

        // private variables

        std::pair< const Matrix<mpw_t>*, const Matrix<mpw_t>* >    m_MPWMatrixAccess;
        const mpw_t                                                m_MaxMPW;

        Matrix<uint16_t>                                           m_FilterXMap, m_FilterYMap;

        // functions for calculating the gradients
        void gradX( const FilterTable<grad_t>& aFilterTable, const Matrix<uint16_t>& aFilterXMap, const Matrix<uint16_t>& aFilterYMap, const Matrix<data_t>& aData, Matrix<grad_t>& aGradX );
        void gradY( const FilterTable<grad_t>& aFilterTable, const Matrix<uint16_t>& aFilterXMap, const Matrix<uint16_t>& aFilterYMap, const Matrix<data_t>& aData, Matrix<grad_t>& aGradY );

        void calculate_gradients();

        double mpw2sigma();

    };

    /** @ class VectorScanDataGradient
     *  @ class for calculating the gradient based on convolution
     */
    template <typename data_t, typename grad_t, typename mpw_t> class FILTERDLL_API VectorScanDataGradient: public ScanDataGradient<data_t, grad_t, mpw_t>
    {
    public:

        /*gradient of a data matrix */
        VectorScanDataGradient( int aAveraging );

        void setData( const std::vector<const Matrix< data_t >*>& aDataVector, const std::pair< const Matrix<mpw_t>*, const Matrix<mpw_t>* >& aMPW );

        const std::vector< std::pair< Matrix<grad_t>, Matrix<grad_t> > > getGradients() const;

        const std::vector<Matrix<grad_t> > getGradientLengthsL1() const;
        const std::vector<Matrix<grad_t> > getGradientLengths() const;
        const std::vector<Matrix<grad_t> > getGradientLengthsSqr() const;
        const std::vector<Matrix<grad_t> > getGradientAngles() const;

        const Matrix<grad_t> getSumOfAllGradientLengthsSqr() const;

    protected:

        std::vector<const Matrix< data_t >*> m_DataVector;

        std::vector< std::pair< Matrix<grad_t>, Matrix<grad_t>>> m_Gradients;

        // private variables
        void calculate_gradients();

    };
}

#endif