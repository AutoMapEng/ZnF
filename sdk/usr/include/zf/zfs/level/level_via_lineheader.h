#pragma once
#include "../zfsapi.h"


#include "level_via_3pos.h"

class ZFSLinebyline;

namespace zfs
{
	ZFS_API
		LevelStehachse calc_level_via_lineheader(const std::wstring& zfs);

	ZFS_API
		LevelStehachse calc_level_via_lineheader(ZFSLinebyline* lbl);


} // end namespace zfs
