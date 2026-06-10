#pragma once
#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include "flt.h"
#include "../mixpix.h"
#include "../../pixel.h"
#include "../../noise.h"

namespace zfs
{

#if 1
    extern zfs::RangeNoise RangeNoiseDataset;          ///< range noise info of data set

    inline bool mixVert(const ZFSFilter& f, double range2, double range1)
    {
        double rej = 0.0;
        if (range1 < range2)
        {
            if (range1)
                rej = ((range2 - range1) * f.cosSubAngleVert) / (range1 * f.sinSubAngleVert);
        }
        else if (range2 < range1)
        {
            if (range2)
                rej = ((range1 - range2) * f.cosSubAngleVert) / (range2 * f.sinSubAngleVert);
        }
        else if (range2 == range1)
            return 0;

        return fabs(rej) > f.threshold;
    }

    inline bool checkConnectedPixel(const ZFSFilter& f, double rg_a, double rg_b, int i_a, int i_b)
    {
        const double ns_a = f.RangeNoiseDataset.calcMM(i_a);
        const double ns_b = f.RangeNoiseDataset.calcMM(i_b);
        const double ns = f.smallObjectsNSigma * std::sqrt(ns_a * ns_a + ns_b * ns_b) + 0.5;

        const double d = fabs(rg_a - rg_b) * 1e3;
        if (d < ns)
        {
            return true;
        }
        else if (mixVert(f, rg_a, rg_b)==false)
        {
            return true;
        }

        return false;
    }

    inline void largeConnectedObjectsSingle(const ZFSFilter& f, PDatas<PDataDetail>& cur,/* const double* const rgCurrent, const uint32_t* const rfCurrent,*/ const bool* const maskIn, bool* const maskOut)
    {
        // -- variables
        int    NObjCnt = 0, NSmallObj = 0;      ///< Cnter for small objects

        const int Npixel = (int)cur.points.size();
        const double smallObjectsFilterConstant = f.smallObjectsExtend / (2. * PI) * Npixel;

        // -- processing
        memset(maskOut, 0, Npixel * sizeof(bool));

        for (int i = 1; i < Npixel; ++i)
        {
            if ( cur.points[i - 1].rg > f.minRg && maskIn[i - 1] &&
                cur.points[i].rg > f.minRg && maskIn[i] &&
                checkConnectedPixel(f,cur.points[i - 1].rg, cur.points[i].rg, cur.points[i - 1].rf, cur.points[i].rf))
            {
                if (NObjCnt == 0)
                {
                    NSmallObj = (int)std::ceil(smallObjectsFilterConstant / cur.points[i - 1].rg);
                    NObjCnt = 2;
                }
                else
                {
                    ++NObjCnt;
                }
            }
            else if (NObjCnt)
            {
                if (NObjCnt >= NSmallObj) // detection larger object
                {
                    const int start_idx = i - NObjCnt;
                    const int end_idx = i - 1;

                    for (int j = start_idx; j <= end_idx; ++j)
                        maskOut[j] = 1;

                }
                NObjCnt = 0;
            }
        }
    }

    // just a single line
    inline void _smallConnectedObjectsSingle(const ZFSFilter& f, PDatas<PDataDetail>& cur, /*const double* const rgCurrent, const uint32_t* const rfCurrent,*/ const bool* const maskIn, bool* const maskOut)
    {
        const int Npixel = (int)cur.points.size();

        int NObjCnt = 0; // Object counter
        int NSmallObj = 0; // Counter for small objects
        const double smallObjectsFilterConstant = f.smallObjectsExtend / (2. * PI) * Npixel;

        memset(maskOut, 0, Npixel * sizeof(bool));

        for (int i = 1; i < Npixel; ++i)
        {
            if (cur.points[i - 1].rg > f.minRg && maskIn[i - 1] &&
                cur.points[i].rg > f.minRg && maskIn[i] &&
                checkConnectedPixel(f, cur.points[i - 1].rg, cur.points[i].rg, cur.points[i - 1].rf, cur.points[i].rf))
            {
                if (NObjCnt == 0)
                {
                    NSmallObj = (int)std::ceil(smallObjectsFilterConstant / cur.points[i - 1].rg);
                    NObjCnt = 2;
                }
                else
                {
                    ++NObjCnt;
                }

            }
            else if (NObjCnt)
            {
                if (NObjCnt < NSmallObj) // detection small object
                {
                    for (int j = i - NObjCnt; j < i; ++j)
                    {
                        if (maskIn[j])
                            maskOut[j] = 1;
                    }
                }

                NObjCnt = 0;
            }
            else if (i > 1 
                && ((!checkConnectedPixel(f, cur.points[i - 2].rg, cur.points[i - 1].rg, cur.points[i - 2].rf, cur.points[i - 1].rf) || !maskIn[i - 2])
                && (!checkConnectedPixel(f, cur.points[i - 1].rg, cur.points[i].rg, cur.points[i - 1].rf, cur.points[i].rf) || !maskIn[i])
                    )
                )// individual pixel
            {
                NSmallObj = (int)std::ceil(smallObjectsFilterConstant / cur.points[i - 1].rg);
                if (NSmallObj > 1 && maskIn[i - 1]) 
                    maskOut[i - 1] = 1;
            }
        }
    }

    // with pre and next
    inline void _smallObjectsFilter(const ZFSFilter& f, PDatas<PDataDetail>& cur, const PDatas<PDataDetail>& pre, const PDatas<PDataDetail>& next,/* const double* const rgPre, const uint32_t* const rfPre, const double* const rgCurrent, const uint32_t* const rfCurrent,
        const double* const rgNext, const uint32_t* const rfNext,*/
        const bool* const maskIn, bool* const maskOut)
    {
        char cur_so_line_extend = 0x02;

        bool within = false;
        int start_idx = 0, stop_idx = 0;

        const int nPixel = (int)cur.points.size();
        if (nPixel == 0) return;

        boost::scoped_array<bool> maskOutLargeO(new bool[nPixel]);

        _smallConnectedObjectsSingle(f, cur, maskIn, maskOut);

        if (f.smallObjectsLines > 1)
        {
            largeConnectedObjectsSingle(f, cur, maskIn, maskOutLargeO.get());

            // check if one of the large objects has larger line extend
            for (int i = 0; i < nPixel; ++i)
            {

                if (maskOutLargeO[i])
                {
                    if (cur_so_line_extend != 0x07)
                    {
                        int line_cnt = 0;

                        if (checkConnectedPixel(f, cur.points[i].rg, pre.points[i].rg, cur.points[i].rf, pre.points[i].rf ))
                            line_cnt += 0x010100;

                        if (checkConnectedPixel(f, cur.points[i].rg, next.points[i].rg, cur.points[i].rf, next.points[i].rf ))
                            line_cnt += 0x000101;

                        if (!i || !within)
                        {
                            start_idx = i;

                            if ((line_cnt & 0x0000ff) && (line_cnt & 0xff0000))
                                cur_so_line_extend = 0x07;
                            else if (line_cnt & 0x0000ff)
                                cur_so_line_extend = 0x03;
                            else if (line_cnt & 0xff0000)
                                cur_so_line_extend = 0x06;

                            within = true;
                        }
                        else if (cur_so_line_extend != 0x07)
                        {
                            if (line_cnt & 0x0000ff)
                                cur_so_line_extend |= 0x03;
                            if (line_cnt & 0xff0000)
                                cur_so_line_extend |= 0x06;
                        }
                    }

                }
                else if (within)
                {
                    stop_idx = i - 1;
                    within = false;
                    bool isSO = false;

                    switch (f.smallObjectsLines) // extend of object to keep => delete section in maskOut for larger objects
                    {
                    case 3:
                    {
                        if (cur_so_line_extend == 0x03 || cur_so_line_extend == 0x06)
                            isSO = true;
                    }
                    case 2:
                    {
                        if (cur_so_line_extend == 0x02)
                            isSO = true;
                        break;
                    }
                    default: break;
                    }

                    for (int j = start_idx; isSO && j <= stop_idx; ++j)
                        maskOut[j] = 1;

                    cur_so_line_extend = 0x02;

                }
            }
        }
    }

#endif


} // end namespace
