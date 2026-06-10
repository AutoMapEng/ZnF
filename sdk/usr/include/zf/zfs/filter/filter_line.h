#pragma once
#include "../zfsapi.h"

#include "filter_param.h"
#include "linebyline_filter.h"
#include "../pixel.h"
#include "filter_queue.h"

class ZFSFilter;

namespace zfs
{
    //##################### filter interface ##############################


    template<typename T>
    LineFilterResult FilterZeile3(const ZFSFilter* param, PDatas<T>& lineCur, const  PDatas<T>& linePre, const  PDatas<T>& lineNext);

    LineFilterResult FilterZeile3(const ZFSFilter* param, PDatasQueue&);

    
    extern template LineFilterResult ZFS_API FilterZeile3<PDataRaw>(   const ZFSFilter* param, PDatas<PDataRaw>&    lineCur, const  PDatas<PDataRaw>&    linePre, const  PDatas<PDataRaw>&    lineNext);
    extern template LineFilterResult ZFS_API FilterZeile3<PData>(      const ZFSFilter* param, PDatas<PData>&       lineCur, const  PDatas<PData>&       linePre, const  PDatas<PData>&       lineNext);
    extern template LineFilterResult ZFS_API FilterZeile3<PDataDetail>(const ZFSFilter* param, PDatas<PDataDetail>& lineCur, const  PDatas<PDataDetail>& linePre, const  PDatas<PDataDetail>& lineNext);
    extern template LineFilterResult ZFS_API FilterZeile3<PDataNormal>(const ZFSFilter* param, PDatas<PDataNormal>& lineCur, const  PDatas<PDataNormal>& linePre, const  PDatas<PDataNormal>& lineNext);
    
} // end namespace
