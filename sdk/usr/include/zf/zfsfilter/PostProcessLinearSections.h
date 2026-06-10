#pragma once

#include "filterapi.h"

#include <vector>

typedef unsigned char byte;

namespace zfsfilter {
    /** \class PostProcessLinearSections
     * \brief  Class for post processing of kalman evaluated lines as used for mixed pixel filtering.
     *
     * \author Juergen Holzner
     *
     * Class for post processing of kalman evaluated lines as used for mixed pixel filtering.
     * Evaluation of lines is conducted according to distance configuration and a rule base.
     *
     */
    template<class T> class FILTERDLL_API PostProcessLinearSections
    {
        template<typename S> friend class MixedPixelKalmanProcessing;

    private:
        // member variables
        T        m_FlatnessThreshold;
        T        m_ScannerTypeNoiseOffset;
        const T *m_ProfileSamplingIntroducedError;

    public:

        PostProcessLinearSections();

        PostProcessLinearSections( const T&              a_FlatnessThreshold,
                                   const T&         a_ScannerTypeNoiseOffset);

        // public member functions
        void init_parameter( const T&              a_FlatnessThreshold,
                             const T&         a_ScannerTypeNoiseOffset);

        void process( const std::vector<T>             &              D_Line,
                      const std::vector<T>             &          Noise_Line,
                      const std::vector<int>           &      Intensity_Line,
                      const std::vector<unsigned short>&            MPB_Line,
                      const std::vector<bool>          & LinearSections_Line,
                      const std::vector<bool>          &          MaskI_Line,
                            std::vector<byte>          &         MPCand_Line);
    };
}
