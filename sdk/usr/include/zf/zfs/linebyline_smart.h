#pragma once

#include "zfsapi.h"

#include "zfserr.h"
#include "../Common/inttypes_.h"

#include <memory>
#include <string>
#include <cstdint>

#include "linebyline.h"

namespace zfs
{
	/**
		OpenLineByLine with smart pointer support
	*/

	typedef std::shared_ptr<ZFSLinebyline> zfslinebyline_ptr;

	ZFS_API zfslinebyline_ptr open_linebyline(const std::wstring& zfsFileName, zfserr_t* err = NULL, std::int32_t openflags = 0);

} // end namespace
