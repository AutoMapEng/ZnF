#pragma once
#include "../zfsapi.h"

#include <memory>

#include "filter_param.h"
#include "filter_result.h"
#include "filter_queue.h"

class ZFScan;
class ZFRect;

namespace zfs
{

	// filter a scan in memory via linebyline filter code
	ZFS_API LineFilterResult filterZFScan(ZFScan*, const std::shared_ptr<FilterParams> params, const ZFRect& rc, bool singleMask );




	ZFS_API bool zfscanQueue(ZFScan* z, const ZFRect& rcImage, PDatasQueue::data_cb_f, bool multithreaded=false);

	ZFS_API bool zfscanQueueWithFilter(ZFScan* z, const ZFRect& rcImage, PDatasQueue::data_cb_f, std::shared_ptr<zfs::FilterParams> params, bool multithreaded = false);

} // end namespace
