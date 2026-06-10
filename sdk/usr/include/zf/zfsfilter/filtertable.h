#pragma once

#include <vector>
#include <utility>
#include <string>

#include "filterapi.h"

#include <zfs/Math/signal_processing/filter_kernels.h>

namespace zfsfilter
{

    //================================================================================================================
    /**
     * @class FilterTable
     * @brief Class for providing filter kernels (smoothing, differentiating) in a filter bank.
     *
     * @author Juergen Holzner
     *
     * Class for providing filter kernels (smoothing, differentiating) in a filter bank.
     *
     */
    template <class T> class FILTERDLL_API FilterTable
    {
      public:
          
          FilterTable( const zfs::math::SignalWindowNameType aWindowName,  const int& aNumberOfKernels, const T& aFilterParameter, const T& aFilterLengthParameter= 0 );

          T getFilterParameter() const;
          T getFilterLengthParameter() const;

          const std::vector<T>& getSmoother( size_t aIdx ) const;
          const std::vector<T>& getDifferentiator( size_t aIdx ) const;

          bool getInitOK() const;
          std::string getError() const;

      private:

          bool        m_bInitOK;
          std::string m_strError;

          T m_FilterParameter, m_FilterLengthParameter;

          std::vector< std::pair< std::vector<T>, std::vector<T> > > m_FilterTableContainer;

    };

}