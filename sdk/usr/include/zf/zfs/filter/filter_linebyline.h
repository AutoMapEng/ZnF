#pragma once
#include "../zfsapi.h"

#include <string>

#include "filter_param.h"
#include "../linebyline.h"
#include "linebyline_filter.h"
#include "../pixel.h"
#include "../Tools/progress.h"

#include "filter_line.h"
#include "filter_queue.h"
#include "filter_execution.h"


namespace zfs
{

	/**
	* /param cpuUsage 0=single threaded 100=use all available cpu
	*/
	ZFS_API bool zfslinebylineQueue(ZFSLinebyline* z, PDatasQueue::data_cb_f, PDatasQueue::act_cb_f, double cpuUsage=0., bool overlap=true, zf::progress_f prog = nullptr);

	//#######################################################################################################################################################################################################

	/**
	* @param cpuUsagePercent 0=single threaded, 100=usa all cpu available
	*/
	ZFS_API ScanFilterReport zfslinebylineQueue_WithFilter(ZFSLinebyline* z, std::shared_ptr<FilterParams> params, PDatasQueue::data_cb_f, double cpuUsagePercent=100, zf::progress_f prog=nullptr );


	ZFS_API ScanFilterReport zfslinebylineQueue_WithFilter_WriteMasks(ZFSLinebyline* z, std::shared_ptr<FilterParams> params, const FilterExecution& executionParams );

	ZFS_API ScanFilterReport zfslinebylineQueue_WithFilter_WriteMasks(const std::wstring& zfsFileName, std::shared_ptr<FilterParams> params, const FilterExecution& executionParams);

	// filter with the params as they stored or use default
	ZFS_API ScanFilterReport zfslinebylineQueue_WithFilter_WriteMasks(const std::wstring& zfsFileName, const FilterExecution& executionParams=FilterExecution());


	//#######################################################################################################################################################################################################

	// filter with the params as they stored or use default
	ZFS_API ScanFilterReport zfslinebylineQueue_WithFilter_WriteLayer(const std::wstring& zfsFileName, const FilterExecution& executionParams = {}, zf::progress_f prog = nullptr);

	ZFS_API ScanFilterReport zfslinebylineQueue_WithFilter_WriteLayer(std::shared_ptr<ZFSLinebyline> lbl, std::shared_ptr<FilterParams> params, const FilterExecution& exeParams = {}, zf::progress_f prog = nullptr);

} // end namespace
