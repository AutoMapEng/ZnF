#pragma once
#include "../zfsapi.h"

#include <string>

#include "../trmatrix.h"

class ZFSHeader;
class ZFSLinebyline;

namespace zfs
{

	//
	// Levelingsystem starting with 5024 serie
	//

	struct LevelSystem
	{
		bool m_valid{};
		int m_error{};
		ZFTRMatrix m_mat;
		enum class mode_t
		{
			UNDEF, ///< nothing
			POS, ///< calculated from 3position measurement
			LINEHEADER,	///< calculated from dyncomp in lineheader
			PARENT, ///< get info from parent scan if scan is a selection child scan
		};
		mode_t m_mode{ mode_t::UNDEF };
	};


	// load already calculated level from zfs-header 360
	ZFS_API
		LevelSystem load_stehachse(const ZFSHeader*);


	// recalculate new level values
	ZFS_API
		LevelSystem recalcLevel( const std::wstring& zfs, bool debug=false );
	ZFS_API
		LevelSystem recalcLevel(ZFSLinebyline* lbl, bool debug=false );


	// update the recalculated values and store it in the zfs-header
	ZFS_API
		bool updateLevel( const LevelSystem& l, ZFSHeader*);
}