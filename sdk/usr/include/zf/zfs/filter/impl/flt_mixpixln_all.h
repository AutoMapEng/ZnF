#pragma once

#include "flt_mixpixln.h"

namespace zfs
{



    // code mehr oder weniger direkt von 3M. Gleiche variablennamen
    class MixPixLineAll
    {
    public:
        struct Pixel
        {
            float x{};
            float y{};
            float d{};
            float w{};
        };
        struct PixelX : public Pixel
        {
            bool mp{};
            bool dp{};
        };
        struct Vor
        {
            bool mp{};
            bool vd{};
            int j{};
        };

        float cic{}; // user parameter

        const float d2r = (float)M_PI / 180.f;
        float wli5 = 5.0f * d2r; // alt 3.0
        float wli2 = 8.0f * d2r; // alt 6.5
        float wli0 = 15.0f * d2r; // alt 9.0

        float gl5 = 0.02f;
        float gl2 = 0.02f;
        float gl0 = 0.02f;

        float ld0 = 2.f * 3.5e-3f;
        float ldiv = 1.5f * 0.6e-3f;

        Pixel Pv;
        PixelX Pm;
        Vor F, F_1, F_2;
        bool fp{};
        float res{};


        MixPixLineAll(const ZFSFilter& mxpx)
        {
            res = static_cast<float>(mxpx.firmmixAR);
            // aktuell für den profiler-mixedpixel test filter
            cic = static_cast<float>(mxpx.mxpxline_wid * res); // = 2. * M_PI / vertical_pixel_360deg

            wli5 = static_cast<float>(mxpx.mxpxline_w5 * d2r);
            wli2 = static_cast<float>(mxpx.mxpxline_w2 * d2r);
            wli0 = static_cast<float>(mxpx.mxpxline_w0 * d2r);

            gl5 = static_cast<float>(mxpx.mxpxline_g5);
            gl2 = static_cast<float>(mxpx.mxpxline_g2);
            gl0 = static_cast<float>(mxpx.mxpxline_g0);

            Pm.x = Pm.y = Pm.d = Pm.w = 0.;
            Pm.mp = Pm.dp = false;

            F.mp = F.vd = false;
            F.j = 0;
            F_1 = F;
            F_2 = F;
            fp = false;
        }



        /**
        * \param i = index des aktuell zu prüfenden Pixels
        * \param lineCur = die komplette Zeile
        */
        template<typename T>
        bool filter(int j, PDatas<T>& lineCur)
        {
            bool flt = false;

            T& cpt = (T&)lineCur.points[j]; // referenz auf das zu bearbeitende Pixel

            Pixel P;
            P.d = static_cast<float>(cpt.rg); // range [meter]
            P.w = static_cast<float>(cpt.angle.first); // vertical [rad]
            if (P.w > M_PI) P.w -= 2. * M_PI;
            P.x = cosf(P.w) * P.d;
            P.y = sinf(P.w) * P.d;

            if (fp)
            {
                Pm = MixCore(P, Pv, Pm);

                if (Pm.mp)
                {
                    fp = true;
                    disable_nooverride(cpt, FILTERID_MIXPIX_LN); // aktuelles pixel ausmaskieren
                    flt = true;
                }

                F_2 = F_1;
                F_1 = F;
                F.mp = Pm.mp;
                F.j = j;

                auto dl = P.d * res * 2.;
                if (dl < 0.0025)
                    dl = 0.0025;
                const auto dd = fabs(P.d - Pv.d );
                F.vd = dd < dl;

                if (F.mp == false && F_1.mp && F.vd)
                {
                    disable_nooverride(lineCur.points[F_1.j], FILTERID_MIXPIX_LN); // voriges pixel ausmaskieren
                    
                    // falls ein anderer filter wie z.B. der rangefilter bereits den punkt herausgefiltert hat
                    // dann wenn dieser filter den filter für ein pixel wieder zurück nehmen will, auf den vorigen wert stellen

                    //disable(lineCur.points[F_2.j], F_2.vd == false ? FILTERID_MIXPIX_LN : 0);
                    auto& pix = lineCur.points[F_2.j];
                    if (F_2.vd == false)
                    {
                        if (pix.filterID == 0)
                            pix.filterID = FILTERID_MIXPIX_LN;
                    }
                    else
                    {
                        if (pix.filterID == FILTERID_MIXPIX_LN)
                            pix.filterID = 0;
                    }
                }
                else if (F.mp && F_1.mp == false)
                {
                    //disable_nooverride(lineCur.points[F_1.j], F_1.vd == false || lineCur.points[F_2.j].filterID == FILTERID_MIXPIX_LN ? FILTERID_MIXPIX_LN : 0);
                    auto& pix = lineCur.points[F_1.j];
                    if (F_1.vd == false || lineCur.points[F_2.j].filterID == FILTERID_MIXPIX_LN)
                    {
                        if (pix.filterID == 0)
                            pix.filterID = FILTERID_MIXPIX_LN;
                    }
                    else
                    {
                        if (pix.filterID == FILTERID_MIXPIX_LN)
                            pix.filterID = 0;
                    }
                }
            }

            fp = true;
            Pv = P;

            return flt;
        }


        inline PixelX MixCore(const Pixel& P, const Pixel& Pv, PixelX Pm)
        {
            try
            {
                float x1, y1, d1, x0, y0, d0, w0;
                bool dp;
                if (P.d > Pv.d)
                {
                    x1 = P.x;
                    y1 = P.y;
                    d1 = P.d;
                    x0 = Pv.x;
                    y0 = Pv.y;
                    d0 = Pv.d;
                    w0 = Pv.w;
                    dp = true;
                }
                else
                {
                    x1 = Pv.x;
                    y1 = Pv.y;
                    d1 = Pv.d;
                    x0 = P.x;
                    y0 = P.y;
                    w0 = P.w;
                    d0 = P.d;
                    dp = false;
                }

                float ds;
                if (Pm.mp && Pm.dp)
                    ds = Pm.d;
                else
                    ds = d0;

                float wli = wli0;
                float gln = gl0;
                if (ds > 5.)
                {
                    wli = wli5;
                    gln = gl2;
                }
                else if (ds > 2.)
                {
                    wli = wli2;
                    gln = gl0;
                }

                if (Pm.mp)
                {
                    wli *= 1.3f;
                    if (ds > 2.f)
                        gln = gln * 0.667f;
                }

                const float dy = y1 - y0;
                const float dx = x1 - x0;
                //const double dd2 = dy * dy + dx * dx;
                const float wg = atan2f(dy, dx);
                float dw = fabs(wg - w0);
                if (dw > (float)M_PI)
                    dw = fabs(dw - 2.f * (float)M_PI);

//                if (dw < wli && dd2>1e-4)
                {
                    float mg;
                    float dg;
                    float dd;
                    if (Pm.mp && Pm.dp)
                    {
                        mg = y1 / x1;
                        dg = Pm.y - Pm.x * mg;
                        dd = fabs(d1 - Pm.d);
                    }
                    else if (Pm.mp && !Pm.dp)
                    {
                        mg = Pm.y / Pm.x;
                        dg = y0 - x0 * mg;
                        dd = fabs(d0 - Pm.d);
                    }
                    else
                    {
                        mg = y1 / x1;
                        dg = y0 - x0 * mg;
                        dd = fabs(d1 - d0);
                    }
                    const float dg2 = dg * dg / (1.f + mg * mg);
                    const float r1 = ld0 + (ldiv + cic) * ds;

                    if ( dg2<r1*r1 && ((dw < wli && dd>gln) || (dw<2.*wli && dd>6.*gln)))
                    {
                        if (!Pm.mp || (Pm.mp && Pm.dp != dp))
                        {
                            Pm.dp = dp;
                            if (dp)
                            {
                                Pm.x = x0;
                                Pm.y = y0;
                                Pm.d = d0;
                            }
                            else
                            {
                                Pm.x = x1;
                                Pm.y = y1;
                                Pm.d = d1;
                            }
                        }
                        else if (Pm.mp && !dp)
                        {
                            Pm.x = P.x;
                            Pm.y = P.y;
                            Pm.d = P.d;
                        }

                        Pm.mp = true;
                    }
                    else
                        Pm.mp = false;
                }
//                else
//                    Pm.mp = false;
            }
            catch (...)
            {
            }

            return Pm;
        }


    };


} // end namespace
