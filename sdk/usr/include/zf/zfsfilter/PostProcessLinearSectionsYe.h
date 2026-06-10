#pragma once

#include "filterapi.h"

#include <vector>

typedef unsigned char byte;

namespace zfsfilter {
    /** \class Segmentation
     * \brief  Class for post processing according to Ye [Paper] of kalman evaluated lines as used for mixed pixel filtering.
     *
     * \author Juergen Holzner
     *
     * Class for post processing according to Ye [Paper] of kalman evaluated lines as used for mixed pixel filtering.
     *
     */
    class FILTERDLL_API PostProcessLinearSectionsYe
    {
    private:

    public:
        PostProcessLinearSectionsYe();
        void process( const std::vector<bool>          & a_LinearSections_Line,  // line indicating the linear sections _starts_ with bool true!
                      const std::vector<unsigned short>&            a_MPB_Line,  // mixed pixel width line
                      std::vector<byte>          &         a_MP_MaskLine); //line with the mixed pixel candidate values: 1: possibly no mixed pixel, 2: possibly mixed pixel, 3: surely mixed pixel
                                                            // here only the values 1 and 3 are used
    };
}
