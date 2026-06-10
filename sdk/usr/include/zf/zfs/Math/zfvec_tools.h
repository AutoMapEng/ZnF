#pragma once

#include "../zfsapi.h"
#include "zfvec.h"

#include <string>

namespace zfs {
	namespace math
	{
        /**
        \brief parse a string like "1.2 2.3 4.45" into a vector
        */
        Vec3d ZFS_API string2vec(const std::string& str, const std::string& sep = " ");

#ifdef _WINDOWS
        Vec3d ZFS_API string2vec(const std::wstring& str, const std::wstring& sep = L" ");
#endif
    }
}
