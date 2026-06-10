#pragma once
#include "zfsapi.h"

#include <string>


namespace zfs
{
	ZFS_API_F
		bool jpegStore(char* buf, int step, int wd, int hg, const std::wstring& fn);

}