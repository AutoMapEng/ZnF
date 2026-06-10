#pragma once
#include "../zfsapi.h"

#include <string>
#include <vector>
#include <tuple>

#include "../trmatrix.h"
#include "level.h"
#include "level_errors.h"

class ZFSHeader;

namespace zfs
{


    /**
     * \brief Berechne die Matrix aus den 3 positionen an denen die Libelle gemessen wurde.
     * New implementation based on old code. Old code has to be removed or still works with 5016 devices
     * 1. 0
     * 2. 180deg
     * 3. optional noch mal 0 deg für check
     */
    class ZFS_API Level3Pos
    {
    public:
        bool calc(const ZFSHeader*);

        bool hasValuesInZfsHeader() const { return m_22000_rad.size() >= 2; }

        const LevelStehachse stehachse() const { return m_result; }
        ZFTRMatrix matrix() const { return m_result.mat; }
        double roll() const { return m_result.Roll_out; }
        double pitch() const { return m_result.Pitch_out; }
        double yaw() const { return m_result.Yaw_out; }
        double tx() const { return m_result.offset_x; }
        double ty() const { return m_result.offset_y; }
        double tz() const { return 0; }

    protected:
        bool read_values_from_zfsheader(const ZFSHeader*);
        int calcLevel_3Positions(const ZFSHeader* head);

        std::vector< std::tuple<double, double, double> >m_22000_rad;	///< x,y,horz [rad]

        LevelStehachse m_result;
    };


    ZFS_API
        LevelStehachse calc_level_via_3pos(const ZFSHeader*);


} // end namespace zfs
