#pragma once
#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include "flt.h"
#include "../mixpix.h"
#include "../../pixel.h"

namespace zfs
{


    template<typename T>
    void overRfFilter_(const ZFSFiltering& mxpx, int pixel, PDatas<T>& line)
    {
        if (mxpx.overloadRfPixMargin) // 5016
        {
            for (int i = -mxpx.overloadRfPixMargin, i_fw = 0; i_fw < pixel; ++i, ++i_fw)
            {
                if (static_cast<int>(line.points[i_fw].rf) == mxpx.overloadRf)
                {
                    for (int i_ = i; i_ <= i_fw; i_++)
                    {
                        if (i_ >= 0 && line.points[i_].filterID != zfs::FILTERID_OVER) // access -> sequence of conditions
                        {
                            line.points[i_].filterID = zfs::FILTERID_OVER;
                        }

                    }

                    // forward to end of section
                    ++i_fw; ++i;
                    while (i < pixel)
                    {
                        if (i >= 0)
                        {
                            if (!(line.points[i].filterID == zfs::FILTERID_OVER
                                || static_cast<int>(line.points[i].rf) == mxpx.overloadRf)) {
                                break;
                            }

                            if (i >= 0 && static_cast<int>(line.points[i].rf) == mxpx.overloadRf)
                            {
                                line.points[i].filterID = zfs::FILTERID_OVER;
                            }
                            ++i_fw;
                        }
                        ++i;
                    }

                    // delete pixels after end of section
                    for (int i_ = i; i_ <= i_fw && i_ < pixel; i_++)
                    {
                        if (i_ >= 0 && line.points[i_].filterID != zfs::FILTERID_OVER)
                        {
                            line.points[i_].filterID = zfs::FILTERID_OVER;
                        }

                        // reset to catch areas between/after closely following pixels
                        if (static_cast<int>(line.points[i_].rf) == mxpx.overloadRf)
                        {
                            i = i_;
                            i_fw = i_ + mxpx.overloadRfPixMargin;
                        }

                    }
                }
            }
        }
        else if (mxpx.overRf)
        {
            // zeile durchgehen und bereiche loeschen
            for (int i = 0; i < pixel; i++)
            {
                //zeile->Set(i);
                const int rfinc = line.points[i].rf;// zeile->Rf();
                if (rfinc > mxpx.overRf)
                {
                    int no = 0;
                    for (int ii = i; ii < pixel; ii++)
                    {
                        //zeile->Set(ii);
                        const int rfinc = line.points[ii].rf; // zeile->Rf();
                        if (rfinc > mxpx.overRf)
                        {
                            no++;
                        }
                        else
                            break;
                    }
                    if (no > 3) no = 3;

                    int start = i - mxpx.overPreInc;
                    if (start < 0) start = 0;
                    i = start;
                    const int cnt = mxpx.overPreInc + mxpx.overPostInc * no;
                    for (int j = 0; j < cnt && i < pixel; i++, j++)
                    {
                        line.points[i].filterID = zfs::FILTERID_OVER;
                    }
                    i++;
                }
            }
        }
    }

} // end namespace
