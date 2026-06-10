#pragma once
#include "../zfsapi.h"


#include "level_filter.h"

class ZFSHeader;


namespace zfs
{
    namespace level
    {


        struct DyncompCheckParams
        {
            bool valid{};
            int max_num_fastChanges{ 3 };
        };

        ZFS_API
            DyncompCheckParams dyncomp_check_params(const ZFSHeader* h);



        /**
        * \brief Check for fast movement
        * 
        * \return number of fast changes.
        */
        struct DyncompCeckResult
        {
            bool fast_movement_detected{};
            int lines_checked{};
            int num_fastChanges{};
        };
        ZFS_API
            DyncompCeckResult dyncomp_dynamic_check(const LevelRaw& filtered, double dyn_lim_deg);


        ZFS_API 
            DyncompCeckResult dyncomp_check_fastmovement(const std::wstring& zfs, const LevelLineheaderCheckParams& p);
    }
} // end namespace zfs
