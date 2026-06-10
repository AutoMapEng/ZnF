#ifndef ZFSFILTER_EQUAL_RECT_H
#define ZFSFILTER_EQUAL_RECT_H

#include "all.h"

class ZFScan;

namespace zfsfilter
{
    FilterResultItem FILTERDLL_API FilterEqual(zfs::ProgressCallbackInterface* cbStatus, ZFScan* zfs, const ZFRect& rc, const ParamsThin& param);
} // end namespace

#endif //ZFSFILTER_EQUAL_RECT_H
