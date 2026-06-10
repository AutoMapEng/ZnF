#pragma once
#include "../zfsapi.h"
#include "rgb.h"

#include <string>

namespace zfs {
	namespace math
	{

        /// string to color value
        rgb ZFS_API string2rgb(const std::string&);
    
        /// color value to a string representation
        std::string ZFS_API rgb2string(const rgb&, const std::string& separator=" " );
    }
}
