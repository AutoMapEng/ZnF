#pragma once
#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include "flt.h"
#include "../mixpix.h"
#include "../../pixel.h"

namespace zfs
{



    //####################################################################

    // Filter pixel heraus die per firmware gesetzt werden
    class FirmwareFilterBits
    {
    public:
        const ZFSFilter& mxpx;

        FirmwareFilterBits(const ZFSFilter& mp)
            : mxpx(mp)
        {
        }

        template<typename T>
        inline bool filter(int i, PDatas<T>& lineCur)
        {
            zfs::PDataDetail& cur = (zfs::PDataDetail&)lineCur.points[i];
            const auto pixel = lineCur.points.size();

            // PS
            if (mxpx.sigproc_stability && (cur.add & 0x40))
            {
                disable(cur, FILTERID_FIRMMIXPIX_2);
                return true;
            }

            // IP
            const uint8_t mask = 0x80;
            if (mxpx.sigproc_IP && (cur.add & mask) && cur.rg)
            {
#if 1
                if (i < static_cast<int>(pixel) - 2)
                {
                    const zfs::PDataDetail& p0 = (zfs::PDataDetail&)lineCur.points[i];
                    const zfs::PDataDetail& p1 = (zfs::PDataDetail&)lineCur.points[i + 1];
                    const zfs::PDataDetail& p2 = (zfs::PDataDetail&)lineCur.points[i + 2];

                    // wenn das nächste auch gesetzt dann ist das pixel definitiv zu löschen
                    // Pixel löschen, falls nächstes oder übernächstes „ungültig“ (entweder „invalid“ Bit gesetzt oder rf <= minrf) ist!
                    if ((p1.add & mask) || (p2.add & mask) ||(static_cast<int>(p1.rf) <=mxpx.minRf) || (static_cast<int>(p2.rf)<=mxpx.minRf) )
                    {
                        disable(cur, FILTERID_FIRM_NONIUS);
                        return true;
                    }

                    const double x0 = p0.y();
                    const double y0 = p0.z();
                    const double x1 = p1.y();
                    const double y1 = p1.z();
                    const double x2 = p2.y();
                    const double y2 = p2.z();

                    const double d0 = p0.rg;
                    const double d1 = p1.rg;

                    const double dx = x2 - x1;
                    const double dy = y2 - y1;

                    const double pr = d1 * mxpx.firmmixAR;

                    const double d01 = fabs(x1 - x0) + fabs(y1 - y0);
                    const double d12 = fabs(x2 - x1) + fabs(y2 - y1);
                    if (dx)
                    {
                        const double m = dy / dx;

                        const double v = (y0 - y1) - (x0 - x1) * m;
                        const double dd = (v * v) / (1. + m * m);

                        if (dd < 4. * pr * pr
                            && d01 < 2. * d12
                            && d12 < 15. * pr
                            && d0 > 0.3)
                        {
                            //nur zum debuggen                                    disable(cur, FILTERID_FIRMMIXPIX_3);
                        }
                        else
                        {
                            disable(cur, FILTERID_FIRM_NONIUS);
                            return true;
                        }
                    }
                    else
                    {
                        disable(cur, FILTERID_FIRM_NONIUS);
                        return true;
                    }
                }
                else // am ende der Zeile
#endif
                {
                    disable(cur, FILTERID_FIRM_NONIUS);
                    return true;
                }
            }

            // WF
            if (mxpx.sigproc_weak && (cur.add & 0x04))
            {
                disable(cur, FILTERID_FIRMMIXPIX_1);
                return true;
            }
            // SF
            if (mxpx.sigproc_strong && (cur.add & 0x08))
            {
                disable(cur, FILTERID_FIRMMIXPIX_3);
                return true;
            }
            return false;
        }

    };


} // end namespace
