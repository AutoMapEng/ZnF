#pragma once

#include "flt_mixpixln.h"
#include "fast_atan2.h"

namespace zfs
{


    //####################################################################

    // code mehr oder weniger direkt von 3M. Gleiche variablennamen
    // Erste Version von 3M
    class MixPixLineProfiler2
    {
    public:

        double cic{};

        const double d2r = M_PI / 180.;
        double wli5 = 3.0 * d2r;
        double wli2 = 6.5 * d2r;
        double wli0 = 9.0 * d2r;

        bool mp = false;
        double xv = 0, yv = 0, dv = 0, wv = 0;
        double xm = 0, ym = 0, dm = 0;
        bool dpm = false;
        int lastIndex{}; // speichert die position die zuletzt mit diesem filter überprüft wurde. Also die Pixel die über min Intensity liegen


        MixPixLineProfiler2(const ZFSFilter& mxpx)
        {
            // aktuell für den profiler-mixedpixel test filter
            cic = mxpx.mxpxline_wid * mxpx.firmmixAR; // = 2. * M_PI / vertical_pixel_360deg

            wli5 = mxpx.mxpxline_w5 * d2r;
            wli2 = mxpx.mxpxline_w2 * d2r;
            wli0 = mxpx.mxpxline_w0 * d2r;
        }

        /**
        * \param i = index des aktuell zu prüfenden Pixels
        * \param lineCur = die komplette Zeile
        */
        template<typename T>
        bool filter(int i, PDatas<T>& lineCur)
        {
            bool flt = false;

            T& cpt = (T&)lineCur.points[i];

            const double x = - cpt.z(); // x im Scannersystem ist die Fahrtrichtung
            const double y = cpt.y();
            const double d = cpt.rg; // meter
            double w = cpt.angle.first; // rad
            if (w > M_PI) w -= 2. * M_PI;

            try
            {
                if (i)
                {
                    double x1, y1, d1, x0, y0, d0, w0;
                    bool dp;
                    if (d > dv)
                    {
                        x1 = x;
                        y1 = y;
                        d1 = d;
                        x0 = xv;
                        y0 = yv;
                        d0 = dv;
                        w0 = wv;
                        dp = true;
                    }
                    else
                    {
                        x1 = xv;
                        y1 = yv;
                        d1 = dv;
                        x0 = x;
                        y0 = y;
                        w0 = w;
                        d0 = d;
                        dp = false;
                    }

                    double mg;
                    double ds;
                    double dg2;
                    double rl;
                    if (mp && dpm)
                    {
                        mg = y1 / x1;
                        ds = dm;
                        double dg2_ = ym - xm * mg;
                        dg2 = (dg2_ * dg2_) / (1. + mg * mg);
                        rl = 10e-3 + (1.5e-3 + cic) * dm;
                    }
                    else if (mp && !dpm)
                    {
                        mg = ym / xm;
                        ds = d0;
                        double dg2_ = y0 - x0 * mg;
                        dg2 = (dg2_ * dg2_) / (1. + mg * mg);
                        rl = 10e-3 + (1.5e-3 + cic) * d0;
                    }
                    else
                    {

                        mg = y1 / x1;
                        ds = d0;
                        double dg2_ = y0 - x0 * mg;
                        dg2 = (dg2_ * dg2_) / (1. + mg * mg);
                        rl = 10e-3 + (1.5e-3 + cic) * d0;
                    }

                    double wli = wli0;
                    if (ds > 5.)
                        wli = wli5;
                    else if (ds > 2.)
                        wli = wli2;

                    if (mp)
                        wli *= 1.5;

//                    const double wg = atan2(y1 - y0, x1 - x0);
                    const double wg = fastatan2(y1 - y0, x1 - x0);

                    double dw = fabs(wg - w0);
                    if (dw > 6.)
                        dw = fabs(dw - 2. * M_PI);

                    if (dw < wli && dg2 < (rl * rl))
                    {
                        if (!mp)
                        {
                            mp = true;
                            // set mask
                            disable(lineCur.points[lastIndex], FILTERID_MIXPIX_LN); // pixel ausmaskieren
                            disable(cpt, FILTERID_MIXPIX_LN); // pixel ausmaskieren
                            dpm = dp;
                            if (dp)
                            {
                                xm = x0;
                                ym = y0;
                                dm = d0;
                            }
                            else
                            {
                                xm = x1;
                                ym = y1;
                                dm = d1;
                            }
                        }
                        else
                        {
                            disable(cpt, FILTERID_MIXPIX_LN); // aktuelles pixel ausmaskieren
                        }
                        flt = true;
                    }
                    else
                    {
                        mp = false;
                        dm = 0;
                    }

                }
            }
            catch (...)
            {
                disable(cpt, FILTERID_MIXPIX_LN); // aktuelles pixel ausmaskieren
                flt = true;
                flt = true;
            }
            xv = x;
            yv = y;
            dv = d;
            wv = w;

            lastIndex = i;
            return flt;
        }

    };



} // end namespace
