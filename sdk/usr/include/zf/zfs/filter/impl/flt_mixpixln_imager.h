#pragma once

#include "flt_mixpixln.h"
#include "fast_atan2.h"

namespace zfs
{


    //####################################################################
#if 0
    // code mehr oder weniger direkt von 3M. Gleiche variablennamen
    class MixPixProfiler2_Im
    {
    public:

        double cic{}; // user parameter

        const double d2r = M_PI / 180.;
        double wli5 = 3.0 * d2r;
        double wli2 = 6.5 * d2r;
        double wli0 = 9.0 * d2r;

        bool mmp = false;
        double vx = 0, vy = 0, vd = 0, vw = 0;
        double mx = 0, my = 0, md = 0;
        bool mdp = false;
        int lastIndex{}; // speichert die position die zuletzt mit diesem filter überprüft wurde. Also die Pixel die über min Intensity liegen

        MixPixProfiler2_Im(const ZFSFilter& mxpx)
        {
            // aktuell für den profiler-mixedpixel test filter
            cic = mxpx.mxpxline_wid * mxpx.firmmixAR; // = 2. * M_PI / vertical_pixel_360deg

            wli5 = mxpx.mxpxline_5 * d2r;
            wli2 = mxpx.mxpxline_2 * d2r;
            wli0 = mxpx.mxpxline_0 * d2r;
        }




        /**
        * \param i = index des aktuell zu prüfenden Pixels
        * \param lineCur = die komplette Zeile
        */
        template<typename T>
        inline bool filter(int i, PDatas<T>& lineCur)
        {
            bool flt = false;
            bool mp = false;

            T& cpt = (T&)lineCur.points[i]; // referenz auf das zu bearbeitende Pixel

            const double d = cpt.rg; // range [meter]
            double we = cpt.angle.first; // vertical [rad]
            if (we > M_PI) we -= 2. * M_PI;
            const double x = d * cos(we);
            const double y = d * sin(we);

            try
            {
                if (i) // ab dem 2. Pixel
                {
                    double x1, y1, d1, x0, y0, d0, w0;
                    bool dp;
                    if (d > vd)
                    {
                        x1 = x;
                        y1 = y;
                        d1 = d;
                        x0 = vx;
                        y0 = vy;
                        d0 = vd;
                        w0 = vw;
                        dp = true;
                    }
                    else
                    {
                        x1 = vx;
                        y1 = vy;
                        d1 = vd;
                        x0 = x;
                        y0 = y;
                        w0 = we;
                        d0 = d;
                        dp = false;
                    }

                    double ds;
                    if (mmp && mdp)
                        ds = md;
                    else
                        ds = d0;

                    double wli = wli0;
                    if (ds > 5.)
                        wli = wli5;
                    else if (ds > 2.)
                        wli = wli2;

                    if (mmp)
                        wli *= 1.5;

                    const double wg = atan2(y1 - y0, x1 - x0);
                    double dw = fabs(wg - w0);
                    if (dw > M_PI)
                        dw = fabs(dw - 2. * M_PI);

                    if (dw < wli )
                    {
                        double mg;
                        double dg;
                        if (mmp && mdp)
                        {
                            mg = y1 / x1;
                            dg = my - mx * mg;
                        }
                        else if (mmp && !mdp)
                        {
                            mg = my / mx;
                            dg = y0 - x0 * mg;
                        }
                        else
                        {
                            mg = y1 / x1;
                            dg = y0 - x0 * mg;
                        }
                        const double dg2 = dg * dg / (1. + mg*mg);
                        const double r1 = 7e-3 + (0.9e-3 + cic) * ds;

                        if (dg2 < r1 * r1)
                        {
                            if (!mmp)
                            {
                                disable(lineCur.points[lastIndex], FILTERID_MIXPIX_LN); // voriges pixel ausmaskieren

                                mdp = dp;
                                if (dp)
                                {
                                    mx = x0;
                                    my = y0;
                                    md = d0;
                                }
                                else
                                {
                                    mx = x1;
                                    my = y1;
                                    md = d1;
                                }
                            }
                            mp = true;
                        }
                    }

                    mmp = mp;

                    if (mp)
                    {
                        disable(cpt, FILTERID_MIXPIX_LN); // pixel ausmaskieren
                        flt = true;
                    }
                }
            }
            catch (...)
            {
                disable(cpt, FILTERID_MIXPIX_LN); // aktuelles pixel ausmaskieren
                flt = true;
            }

            // das gerade bearbeitete Pixel als voriges speichern fuer den naechsten Durchlauf
            vx = x;
            vy = y;
            vd = d;
            vw = we;

            lastIndex = i;
            return flt;
        }

    };
#endif


    //####################################################################

// code mehr oder weniger direkt von 3M. Gleiche variablennamen
#if 0
    class MixPixLineImager3
    {
    public:
        struct Pixel
        {
            double x {};
            double y {};
            double d {};
            double w {};
        };
        struct PixelX : public Pixel
        {
            bool mp {};
            bool dp {};
        };
        struct Vor
        {
            bool mp {};
            bool vd {};
            int j {};
        };

        double cic{}; // user parameter

        const double d2r = M_PI / 180.;
        double wli5 = 5.0 * d2r; // alt 3.0
        double wli2 = 8.0 * d2r; // alt 6.5
        double wli0 = 15.0 * d2r; // alt 9.0

        Pixel Pv;
        PixelX Pm;
        Vor F, F_1, F_2;
        bool fp{};
        double res{};


        MixPixLineImager3(const ZFSFilter& mxpx)
        {
            res = mxpx.firmmixAR;
            // aktuell für den profiler-mixedpixel test filter
            cic = mxpx.mxpxline_wid * res; // = 2. * M_PI / vertical_pixel_360deg

            wli5 = mxpx.mxpxline_w5 * d2r;
            wli2 = mxpx.mxpxline_w2 * d2r;
            wli0 = mxpx.mxpxline_w0 * d2r;

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
            P.d = cpt.rg; // range [meter]
            P.w = cpt.angle.first; // vertical [rad]
            if (P.w > M_PI) P.w -= 2. * M_PI;
#if 0
            P.x = P.d * cos(P.w);
            P.y = P.d * sin(P.w);
#else
            P.x = cpt.xyz.y();// P.d* cos(P.w);
            P.y = -cpt.xyz.z(); // P.d* sin(P.w);
#endif

            if (fp)
            {
                Pm = MixCore(P, Pv, Pm);

                if (Pm.mp)
                {
                    fp = true;
                    disable(cpt, FILTERID_MIXPIX_LN); // aktuelles pixel ausmaskieren
                    flt = true;
                }

                F_2 = F_1;
                F_1 = F;
                F.mp = Pm.mp;
                F.j = j;

                double dl;
                if (P.d > 2.)
                    dl = P.d * res * 2.;
                else
                    dl = 0.0025;
                F.vd = P.d < dl;

                if (F.mp == false && F_1.mp && F.vd)
                {
                    disable(lineCur.points[F_1.j], FILTERID_MIXPIX_LN); // voriges pixel ausmaskieren
                    disable(lineCur.points[F_2.j], F_2.vd==false ? FILTERID_MIXPIX_LN : 0);
                }
                else if (F.mp && F_1.mp == false)
                {
                    disable(lineCur.points[F_1.j], F_1.vd==false ? FILTERID_MIXPIX_LN : 0);
                }
            }

            fp = true;
            Pv = P;

            return flt;
        }


        inline PixelX MixCore( const Pixel& P, const Pixel& Pv, PixelX Pm)
        {
            try
            {
                double x1, y1, d1, x0, y0, d0, w0;
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

                double ds;
                if (Pm.mp && Pm.dp)
                    ds = Pm.d;
                else
                    ds = d0;

                double wli = wli0;
                if (ds > 5.)
                    wli = wli5;
                else if (ds > 2.)
                    wli = wli2;

                if (Pm.mp)
                    wli *= 1.5;

                const double dy = y1 - y0;
                const double dx = x1 - x0;
                const double dd2 = dy * dy + dx * dx;
//                const double wg = atan2f(dy, dx);
                const double wg = fastatan2(static_cast<float>(dy), static_cast<float>(dx));
                double dw = fabs(wg - w0);
                if (dw > M_PI)
                    dw = fabs(dw - 2. * M_PI);

                if (dw < wli && dd2>1e-4)
                {
                    double mg;
                    double dg;
                    if (Pm.mp && Pm.dp)
                    {
                        mg = y1 / x1;
                        dg = Pm.y - Pm.x * mg;
                    }
                    else if (Pm.mp && !Pm.dp)
                    {
                        mg = Pm.y / Pm.x;
                        dg = y0 - x0 * mg;
                    }
                    else
                    {
                        mg = y1 / x1;
                        dg = y0 - x0 * mg;
                    }
                    const double dg2 = dg * dg / (1. + mg * mg);
                    const double r1 = 7e-3 + (0.9e-3 + cic) * ds;

                    if (dw<wli && dg2 < r1 * r1 && dd2>4e-4 )
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
                else
                    Pm.mp = false;
            }
            catch (...)
            {
            }

            return Pm;
        }


    };
#endif


} // end namespace
