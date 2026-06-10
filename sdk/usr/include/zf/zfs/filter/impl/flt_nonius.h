#pragma once
#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include <vector>

#include "flt.h"
#include "../mixpix.h"
#include "../../pixel.h"

namespace zfs
{

    struct HelpRg
    {
        unsigned int mNr;
        double mRg;
    };

    inline void getNoniusMultiplier(const double& dD,
        const double& Coarse, //const int Coarse_search_range[],
        const double& Fine, //const int Fine_search_range[],
        int32_t& nCoarse, int32_t& nFine, double& dDrem)
    {

        nCoarse = 0;
        nFine = 0;
        dDrem = 0.;

        nCoarse = int(dD / Coarse + 0.5);
        nFine = int((dD - nCoarse * Coarse) / Fine + 0.5);

#ifdef _NON_MULT_CHECK
        if (nCoarse > SearchRangedNoniusCoarse[1] || nCoarse < SearchRangedNoniusCoarse[0] ||
            nFine > SearchRangedNoniusFine[1] || nFine < SearchRangedNoniusFine[0])
        {
            nCoarse = 0;
            nFine = 0;
        }
        else
        {
            dDrem = dD - (nCoarse * Coarse + nFine * Fine);
        }
#else
        dDrem = dD - (nCoarse * Coarse + nFine * Fine);
#endif

    }

    // -- common helper function to nonius jump correction to save code length
    inline double DoNoniusJmpCorrectionOp(const std::vector<double>& rgVec, const double& CurrentPixelRange, const double& noniusAddDist, const unsigned int& nNoniusS, const zfs::DeviceInfo& devInfo)
    {
        const unsigned int filterWindowElements = (unsigned int)rgVec.size();

        if (filterWindowElements > 1)
        {
            // -- constants
            const unsigned int filterWindowElements_m1 = filterWindowElements - 1;
            const unsigned int nNoniusS_m1 = nNoniusS - 1;

            // -- variables
            std::vector<HelpRg> rgN(filterWindowElements_m1);

            // -- processing

            // -- die am meisten vorkommende entfernung suchen
            for (unsigned int w = 0; w < filterWindowElements_m1; ++w)
            {
                HelpRg& rgNw = rgN[w];

                const double CurRefRg = rgVec[w];
                rgNw.mNr = 1;
                rgNw.mRg = CurRefRg; // initialization with reference range
                for (unsigned int v = w + 1; v < filterWindowElements; ++v) // compare to other ranges (avoid double check)
                {
                    if (fabs(CurRefRg - rgVec[v]) < noniusAddDist)       // check if current range in inner loop is within the given tolerance with respect to the range in the outer loop
                    {                                                      // if yes:
                        ++(rgNw.mNr);                                    // increment number and rnage for later mean range determination
                        rgNw.mRg += rgVec[v];
                    }
                }
                if (rgNw.mNr == filterWindowElements) return 0.; // all ranges approximatly the same
                if (rgNw.mNr > 1) rgN[w].mRg /= rgN[w].mNr;
            }

            double       rgChoose = 0;
            unsigned int mx = nNoniusS_m1;
            for (unsigned int w = 0; w < rgN.size(); w++)
            {
                if (rgN[w].mNr > mx)
                {
                    mx = rgN[w].mNr;
                    rgChoose = rgN[w].mRg;
                }
            }

            // es muessen genuegend ok punke vorhanden sein, punkte auf gleicher ebene
            if (mx < nNoniusS)
                return 0.;

            if ( /*rgChoose<devInfo.dNonius2 ||*/ rgChoose > (devInfo.dMaxRange - devInfo.dNonius2))
                return 0.;

            if (fabs(CurrentPixelRange - rgChoose) < noniusAddDist)
            {
                // wert muss nicht veraendert werden, ist ok
                return 0.;
            }
            else
            {

                int non_mult_fak1 = 0, non_mult_fak2 = 0;

                const double dOrg = rgChoose - CurrentPixelRange;
                double dDrem = 0.; // remaining after nonius correction try
                double offset = 0;

                // -- check for best nonius multipliers, given dOrg:
                getNoniusMultiplier(dOrg,
                    devInfo.dNonius1, //SearchRangedNoniusCoarse, // smaller nonius: ~0.72 m
                    devInfo.dNonius2, //SearchRangedNoniusFine, // larger nonius: ~11.53 m
                    non_mult_fak1, non_mult_fak2, dDrem);

                if (std::fabs(dDrem) < noniusAddDist) // check goodness of found multipliers and if OK, modify offset
                {
                    offset = non_mult_fak1 * devInfo.dNonius1 + non_mult_fak2 * devInfo.dNonius2;
                }

                if (offset != 0)
                {
                    const double rgNeu = CurrentPixelRange + offset;
                    if (rgNeu > 0 && rgNeu <= devInfo.dMaxRange)
                        return rgNeu;
                }
            }
        }
        return 0;
    }



} // end namespace
