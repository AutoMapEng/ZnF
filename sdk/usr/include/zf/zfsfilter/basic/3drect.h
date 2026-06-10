#pragma once

#include <zfsfilter/filterapi.h>

#include <string>
#include <vector>

#include "all.h"
#include <zfs/Math/zfvec.h>

class ZFScan;
class ZFTRMatrix;
namespace zfs {
    class ProgressCallbackInterface;
}
namespace zfsfilter {

    class FILTERDLL_API FilterBox {
    public:
        bool in = true;
        
        double xMax = 0.0;
        double yMax = 0.0;
        double zMax = 0.0;
        double xMin = 0.0;
        double yMin = 0.0;
        double zMin = 0.0;

        ZFTRMatrix rot;
    };

    FilterResultItem FILTERDLL_API Filter3DRect(
        zfs::ProgressCallbackInterface* cbStatus,
        ZFScan* zfs,
        std::list<FilterBox> boxes,
        bool useSelection,
        const std::wstring& _maskName = L"");
       
    FilterResultItem FILTERDLL_API Filter3DRect(zfs::ProgressCallbackInterface* cbStatus, ZFScan*,
        bool in, double dx, double dy, double dz, const ZFTRMatrix& rot, bool useSelection,
        const std::wstring& _maskName = L"");


    FilterResultItem FILTERDLL_API Filter3DRect(const std::vector<std::wstring>& zfs,
        bool in, const zfs::math::Vec3d&, const ZFTRMatrix& rot, bool useSelection,
        const std::wstring& _maskName = L"");

} // end namespace

