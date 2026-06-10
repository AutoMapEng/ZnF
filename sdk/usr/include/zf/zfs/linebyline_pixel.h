#pragma once
#include "zfsapi.h"
#include "config.h"

#include "linebyline.h"
#include "pixel.h"
#include "src/spline/spline_.h"


namespace zfs
{

    /**
    \brief  Fast access for pixel in the current line
            This is independed from ZFSLinebyline for better compile performance
    */
    class ZFSLinebylinePixel
    {
    public:
        ZFSLinebylinePixel(const ZFSLinebyline* l) : lbl(l) {}
        /// return range and intensity of all pixel in the line. Use zfs::PDataRaw or derivate
        template<class T>
        zfs::PDatas<T> getRawPixelOfLine( int subpixel=1 ) const;

        /// return range, intensity, xyz and rgb of all pixel in the line.
        template<class T>
        zfs::PDatas<T> getPixelOfLine(int subpixel=1 ) const;

    protected:
        const ZFSLinebyline* lbl;
    };











    template<class T>
    inline zfs::PDatas<T> ZFSLinebylinePixel::getRawPixelOfLine(int subpixel) const
    {
        zfs::PDatas<T> pts;
        const int n = int((double)lbl->Pixel() / subpixel + 0.5);
        pts.points.resize(n);
        const auto ln = lbl->L();
        pts.minrf = ln->Info()->minRf;
        pts.refRg = ln->RefRg();
        pts.refRf = ln->RefRf();
        pts.rgoffset = ln->MOffset();
        for (int i = 0, j = 0; i < n; ++i, j += subpixel)
        {
            T& pt = pts.points[i];
            ln->Set(j);
            pt.rf = ln->Rf();
            pt.rg = ln->Rgm(pts.rgoffset);
            if (lbl->pRfComp)
                pt.rfNorm1 = static_cast<float>(lbl->pRfComp->RfComp(pt.rf, pts.refRf, pt.rg));
            else
                pt.rfNorm1 = static_cast<float>(ln->RfLinear1());
            if (pt.rfNorm1>1.f) pt.rfNorm1 = 1.f;

            if (pt.rf >= pts.minrf && ln->IsSet(i))
            {
            }
            else
            {
                pt.mask |= zfs::PDataRaw::MASK_FILTERED_OUT;
            }

        }
        return pts;
    }

    template<class T>
    inline zfs::PDatas<T> ZFSLinebylinePixel::getPixelOfLine(int subpixel) const
    {
        zfs::PDatas<T> pts;
        const int n = int((double)lbl->Pixel() / subpixel + 0.5);
        pts.points.resize(n);
        const auto ln = lbl->L();
        pts.minrf = ln->Info()->minRf;
        pts.refRg = ln->RefRg();
        pts.refRf = static_cast<int>(ln->RefRf());
        pts.rgoffset = ln->MOffset();
        for (int i = 0, j=0; i < n; ++i, j+=subpixel)
        {
            T& pt = pts.points[i];
            ln->Set(j);
            pt.rf = ln->Rf();
            pt.rg = ln->Rgm(pts.rgoffset);
            if (lbl->pRfComp)
                pt.rfNorm1 = static_cast<float>(lbl->pRfComp->RfComp(pt.rf, pts.refRf, pt.rg));
            else
                pt.rfNorm1 = static_cast<float>(ln->RfLinear1());
            if (pt.rfNorm1>1.f) pt.rfNorm1 = 1.f;

            if (pt.rf >= pts.minrf && ln->IsSet(i))
            {
                // XYZ
                lbl->model->XYZ0(ln, pt.rg, pt.xyz.x(), pt.xyz.y(), pt.xyz.z());
                lbl->PostXYZ0Action(pt.xyz.x(), pt.xyz.y(), pt.xyz.z(), ln);
            }
            else
            {
                pt.mask |= zfs::PDataRaw::MASK_FILTERED_OUT;
            }

            //RGB
            pt.rgb = lbl->rf.rgbrgb(ln, pt);

        }
        return pts;
    }

} // end namespace