#pragma once

#include "timeapi_basic.h"

#ifdef _WINDOWS

#include <string>
#include <memory>



class ZFSLinebyline;

namespace zfs
{
	class TimeAPI;

	struct TimeAPI_Check
	{
		bool finalTime{ true };
		bool raw{ true };

		bool compare{true}; ///< compare the raw time with the final time
	};

	/**
	 * Check result of TimeAPI.
	 */
	ZFS_API std::string timeapi_check(std::shared_ptr<ZFSLinebyline> lbl, const std::shared_ptr<TimeAPI>, const TimeAPI_Check& );
	ZFS_API std::string timeapi_check(ZFSLinebyline* lbl, const TimeAPI*, const TimeAPI_Check&);

}
#endif // _WINDOWS
