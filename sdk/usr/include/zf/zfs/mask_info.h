#pragma once

#include "zfsapi.h"

#include <string>
#include <vector>
#include "Math/rgb.h"

#include "mask_io.h"

class ZFSHeader;
class ZFMask;
class ZFMaskPartial;

namespace zfs
{
	/**
	* \brief The mask infos read from the zfs-header
	*/
	class ZFS_API ZFSMaskInfo
	{
	public:
		std::wstring name;
		zfs::math::rgb color;
		int typ{ MASK_ID_GLOBAL };
		int filterID{}; ///< see filter_id.h
		int flags{0}; ///< flags like do not remove mask while filtering

		uint32_t x{0};	///< start x
		uint32_t y{0};	///< start y
		uint32_t wd{0};	///< width
		uint32_t hg{0};	///< height
		uint64_t addr{0};///< start address in file
		uint32_t zip{1};

		uint32_t filesize{ 0 };	///< bytes used in file
	};


	ZFS_API std::vector< ZFSMaskInfo > readMaskInfos(const std::wstring& zfsfn );
	ZFS_API std::vector< ZFSMaskInfo > readMaskInfos( const ZFSHeader*);

	// rewrite the header structure (low level function)
	ZFS_API bool writeMaskInfos(const std::vector< ZFSMaskInfo >&, ZFSHeader*);


	// fill ZFSMaskInfo structure
	ZFS_API ZFSMaskInfo fillMaskInfo(const ZFMask*);
	ZFS_API ZFSMaskInfo fillMaskInfo(const ZFMaskPartial*);
}