#pragma once
#ifdef _WINDOWS 
#include "../filterapi.h"
#include <string>
#include <zfs/Tools/progress.h>

class ZFSLinebyline;

namespace zfsfilter
{
	FILTERDLL_API bool mxpyln_horz(const std::wstring& zfs);

} // end namespace

#endif // _WINDOWS
