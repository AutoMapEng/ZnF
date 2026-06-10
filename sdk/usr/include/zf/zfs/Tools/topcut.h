#pragma once

#include "../zfsapi.h"

#include <string>


namespace zfs
{
	ZFS_API std::wstring topcutFilename(const std::wstring& scanFileName);

	/// check if there is already a topview for the scan
	ZFS_API bool isTopcutAvailable(const std::wstring& scanFileName);

} // end namespace zfs
