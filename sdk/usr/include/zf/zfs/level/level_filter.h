#pragma once
#include "../zfsapi.h"

#include <string>

#include "level_io.h"

class ZFSHeader;

namespace zfs
{
    namespace level
    {
        /**
         * Filterung der Libellenwerte die im zfs lineheader gespeichert sind.
         */

        struct LevelLineheaderFilterParams
        {
            bool valid{};
            std::string hardware{}; // die hardware der libelle 1=libelle 3=imu
            int scantime_sec{};
            double rps{ 25 };
            int max_num_fastChanges{ 3 };
            int filter_d{};			// size of filter kernel
            double X0_x[3]{};			// x-level correction parameter to eliminate impacte of centrifugal force
            double X0_y[3]{};			// y-level correction parameter to eliminate impacte of centrifugal force
        };

        ZFS_API
            LevelLineheaderFilterParams determine_level_params(const ZFSHeader* head);


        ZFS_API
            bool filterLevel(const LevelRaw& raw, const LevelLineheaderFilterParams& prm, LevelRaw& filtered);

    }
} // end namespace zfs
