#pragma once

#include "config.h"
#include "zfsapi.h"

#include "way_defines.h"


class ZFSHeader;
class ZFLineAccess;
class ZFScanZFS;


namespace zfs
{
    /**
    * @brief Calculates the x-direction of the way the scanner moves in profiler mode.
            The way information can come from different sources like a const space or from an encoder.
    */
    class ZFS_API WayCounting
    {
    public:
        WayCounting();

        /// detect the mode stored in the zfs-header
        static way_t DetectWayModeFromZFSHeader(ZFSHeader* h, bool& found31);
        static way_t DetectWayModeFromZFSHeader(ZFSHeader* h);

        /// return a string representation of the mode
        static const char* mod2String(way_t);

        bool InitWay(ZFSHeader*, way_t mod, double step = 0.1, bool helix = true);
        void InitWay(const WayCounting&);
        /// return current mode used
        way_t getMod() const;
        void setMod(way_t m) { if (m >= WAY_NO && m < _WAY_MAX_) selWay = m; }

        /// calculate the way [meter]
        double CalcWay(int line, double pixel) const;
        double CalcWayWithHelix(int line, double pixel) const;
        double CalcWayWithoutHelix(int line) const;

        /// get the additional x-way [meter]
        double GetWay(const ZFLineAccess* line) const;	///< way in x direction [meter]
        double GetWay(const ZFScanZFS* z) const;

        // enable disable way calculation
        void enableWayCalculating(bool en);
        bool isWayCalculatingEnabled() const;

        /// calculation with helix
        bool useHelix() const;
        void setUseHelix(bool e) { wayHelix = e; }

        double getInc() const { return wayInc; }
        void setInc( double i) { wayInc=i; }

        double getDist() const { return wayDist; }
        void setDist(double d) { wayDist = d; }

    protected:
        bool en{};        ///< enable calculation, otherwise return 0
        way_t selWay{ WAY_UNDEF };	///< choose way interpretation (choose way or xAxis but not both)
        double wayInc{1};	///< increments per rotation
        double wayDist{1}; ///< distance [meter]
        bool wayHelix{true};	///< helix mit einrechnen
    };


    //###################################################################################
    
    inline void WayCounting::enableWayCalculating(bool en_)
    {
        en = en_;
    }

    inline bool WayCounting::isWayCalculatingEnabled() const
    {
        return en;
    }
    
    inline bool WayCounting::useHelix() const
    {
        return wayHelix;
    }

    inline way_t WayCounting::getMod() const
    {
        return selWay;
    }


    inline double WayCounting::CalcWayWithHelix(int line, double pixel) const
    {
        try {
            return (double)line * wayDist / wayInc + pixel * wayDist / wayInc;
        }
        catch (...)
        {
            return 0.;
        }
    }

    inline double WayCounting::CalcWayWithoutHelix(int line) const
    {
        try {
            return (double)line * wayDist / wayInc;
        }
        catch (...)
        {
            return 0.;
        }
    }

    inline double WayCounting::CalcWay(int line, double pixel) const
    {
        if (!en) return 0;

        if (wayHelix)
            return CalcWayWithHelix(line, pixel);
        else
            return CalcWayWithoutHelix(line);
    }
} // end namespace zfs

