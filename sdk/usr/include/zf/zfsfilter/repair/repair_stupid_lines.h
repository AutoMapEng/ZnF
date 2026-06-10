#pragma once
#include "../filterapi.h"

#include <string>
#include <utility>

namespace zfsfilter
{
	FILTERDLL_API std::pair<int,std::wstring> repair_stupid_lines(const std::wstring& zfsFileName);
}
