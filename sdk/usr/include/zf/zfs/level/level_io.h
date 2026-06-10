#pragma once
#include "../zfsapi.h"

#include <vector>
#include <tuple>
#include <string>
#include <algorithm>

#include "../zfs.h"
#include "../linebyline.h"
#include "level_errors.h"

namespace zfs
{
	namespace level
	{
		const std::string Hardware_Org{ "1" };
		const std::string Hardware_Digipas{ "2" };
		const std::string Hardware_IMU{ "3" };

		//######################################################################################
		

		// raw values from line header of a zfs file
		// angle = rad
		struct LevelRaw
		{
			double speed_rps{};	///< rotation speed scan was taken
			double horz_deg{};	///< horz angle of scan. Can be more that 360deg
			std::string hardware{}; // die hardware der libelle 1=libelle 3=imu
			double working_range_deg{}; ///< the max values we will get from the level
			double calib_range_deg{}; ///< calib range can be less that working range 
			double dist_laser_level{}; // height between level and laser zero [meter]


			std::vector< float > x_rad; ///< [rad]
			std::vector< float > y_rad; ///< [rad]
			std::vector< float > h_rad; ///< horz [rad]

			double min_x_rad{360};
			double max_x_rad{-360};
			double min_y_rad{360};
			double max_y_rad{-360};
			double min_h_rad{360};
			double max_h_rad{ -360};

			void reserve(int sz)
			{
				x_rad.reserve(sz);
				y_rad.reserve(sz);
				h_rad.reserve(sz);
			}
			void resize(int sz)
			{
				x_rad.resize(sz);
				y_rad.resize(sz);
				h_rad.resize(sz);
			}

			int size() const { return (int)std::min(x_rad.size(),y_rad.size()); }

			void mm(double x_, double y_, double horz_)
			{
				min_x_rad = std::min(min_x_rad, x_);
				min_y_rad = std::min(min_y_rad, y_);
				min_h_rad = std::min(min_h_rad, horz_);
				max_x_rad = std::max(max_x_rad, x_);
				max_y_rad = std::max(max_y_rad, y_);
				max_h_rad = std::max(max_h_rad, horz_);
			}

			void push_back(double x_, double y_, double horz_)
			{
				mm(x_, y_, horz_);

				x_rad.push_back(static_cast<float>(x_));
				y_rad.push_back(static_cast<float>(y_));
				h_rad.push_back(static_cast<float>(horz_));
			}
		};




		ZFS_API
			bool read_raw_level(ZFScan*, LevelRaw&);

		ZFS_API
			bool read_raw_level(ZFSLinebyline*, LevelRaw&);

		ZFS_API
			bool read_raw_level(const std::wstring&, LevelRaw&);


		//==========================================================================================

		struct LevelLineheaderCheckParams
		{
			double maxTiltAngle_deg{ 0.5 };             // "compensator_max_tilt_angle" = 0.5
			double maxMovementTilt_deg{ 0.007 };          // "compensator_max_movement_tilt" = 0.007
		};
		struct LevelLineheaderCheckResult
		{
			bool valid{};
			int num_fastChanges{};
			int num_outOfRange{};
		};
		ZFS_API
			LevelLineheaderCheckResult raw_level_check( const LevelRaw&, const LevelLineheaderCheckParams& );

		//==========================================================================================

		ZFS_API
			bool raw_level_write_json(const LevelRaw&, const std::wstring& fn);

		ZFS_API
			bool raw_level_read_json(const std::wstring& fn, LevelRaw&);

		//==========================================================================================

		ZFS_API
		eErrorCode checkLevelMinMax(const LevelRaw& raw, double level_max_deg);

	}

} // end namespace zfs
