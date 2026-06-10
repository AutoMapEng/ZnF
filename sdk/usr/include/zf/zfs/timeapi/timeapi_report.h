#pragma once

#include <functional>

#include "timeapi_basic.h"

#ifdef _WINDOWS

class ZFSHeader;

namespace zfs
{
	typedef std::function< void(const std::string&) > report_cb;

	ZFS_API void ReportGlobal(report_cb cb);
	ZFS_API void ReportZFSHeaderInfos(const ZFSHeader& zhead, report_cb cb, bool timeApiRelevantValues = true);

	/*
	ZFS_API void ReportCall(const TimeApiParams&, report_cb cb);
	ZFS_API void ReportVars(const TimeApi& a, report_cb cb);
	ZFS_API void printList(const std::vector<LineCounterInfo>& lst, report_cb cb);
	ZFS_API void ReportParts(const TimeApi& a, report_cb cb);
	ZFS_API void ReportSyncedLines(TimeApi& api, report_cb cb);
	ZFS_API void ReportPause(TimeApi& api, report_cb cb);
	*/
}
#endif // _WINDOWS
