#pragma once

#include "zfsapi.h"
#include "zfserr.h"

#include <memory>
#include <string>
#include <cstdint>


#include "zfscan.h"


namespace zfs
{
	typedef std::shared_ptr<ZFScan> zfscan_ptr;

	ZFS_API zfscan_ptr zfsOpen_inMemory(const std::wstring& zfsFileName, zfserr_t* err = nullptr, std::int32_t openflags = 0);

} // end namespace
