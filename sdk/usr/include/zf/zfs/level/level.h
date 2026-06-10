#pragma once

#include "compensator.h"
#include "calclevel.h"


class ZFSHeader;
class ZFScan;
class ZFSLinebyline;

namespace zfs
{

	const double min_lencZ_field_deg{ 50. - 0.1 }; ///< the min angle required to use the dynamic compensator  [deg]



	struct LevelStehachse
	{
		bool valid{};
		ZFTRMatrix mat;

		// obsolete, will be removed in future
		double Roll_out{};
		double Pitch_out{};
		double Yaw_out{};
		double offset_x{};
		double offset_y{};
	};




	/**
	* \brief holds relevant level information
	*/
	struct ZFS_API LevelInfo
	{
		enum class stat_t : uint32_t
		{
			NO,				///< no information
			LEVEL,			///< 2/3 point level measurement
			COMPENSATOR,	///< level measurement with compensator info in lineheader
			GYRO,			///< level out of working range, use inaccurate gyro information
		};
		int v9009{};	///< zfs header value
		stat_t stat{ stat_t::NO };
		double xDeg{};	///< x direction [deg]
		double yDeg{};	///< y direction [deg]

		/// format info to a string
		std::wstring toString() const;
	};


	/// get level information
	ZFS_API LevelInfo getLevelInfo(const ZFScan*);
	ZFS_API LevelInfo getLevelInfo(const ZFSLinebyline*);


	/// read stehachse from parent if it is a child and a valid parent is available
	ZFS_API
		LevelStehachse getParentLevel(const ZFSHeader* head);

}