#pragma once
#ifdef _WINDOWS 
#include <zfsfilter/filterapi.h>
#include <zfs/filter/filter_result.h>
#include <string>

class ZFSLinebyline;

namespace zfsfilter
{
    class FILTERDLL_API QualityParam
    {
    public:
    };
    /**
    \brief ausduenfilter linebyline based
    */
    FILTERDLL_API zfs::FilterResult qualityFilter(const std::wstring& zfs, const QualityParam& prm= QualityParam() );
    FILTERDLL_API zfs::FilterResult qualityFilter(ZFSLinebyline*, const QualityParam& prm = QualityParam());

} // end namespace

#endif // _WINDOWS
