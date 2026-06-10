#pragma once
#include "../zfsapi.h"

#include "filter_param.h"
#include "filter_id.h"

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <utility>


namespace zfs
{


	typedef std::map< std::string, std::pair<int,std::wstring> > filtercount_t;
	ZFS_API
		void build_list_possible_filter(const std::shared_ptr<FilterParams> fprms, filtercount_t& ids);

	ZFS_API
		std::map<std::wstring, std::shared_ptr<FilterParams> > getFilterTemplates();

	ZFS_API int filterCompare(const std::shared_ptr < FilterParams>, const std::shared_ptr < FilterParams> );

}
