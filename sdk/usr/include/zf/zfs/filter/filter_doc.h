#pragma once
#include "../zfsapi.h"

#include <string>

namespace zfs
{

	// create a docbook output
	ZFS_API std::string filterDocBasics();
	ZFS_API bool filterDocBasics( const std::wstring& textfile );

} // end namespace
