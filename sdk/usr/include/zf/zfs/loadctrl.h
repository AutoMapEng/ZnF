#pragma once

#include "zfsapi.h"

namespace zfs
{

    /// tells if there is already a scan reading
    ZFS_API bool isLoading();

    /// kill a current reading
    ZFS_API void killLoading();

    // intern

    /// check if killing is active
    bool isKilling();


    /**
    \brief handling reading the scan
    */
    class Loading
    {
    public:
        Loading();
        ~Loading();
    };
}
