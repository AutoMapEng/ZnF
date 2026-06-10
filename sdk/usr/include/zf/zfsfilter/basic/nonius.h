#ifndef ZFSFILTER_NONIUS_H
#define ZFSFILTER_NONIUS_H

#include <zfsfilter/filterapi.h>

class ZFScan;

namespace zfsfilter
{
    int FILTERDLL_API Filter_Nonius(ZFScan* zf, bool bMask = true);
} // end namespace

#endif // ZFSFILTER_NONIUS_H
