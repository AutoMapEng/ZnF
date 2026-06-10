#pragma once
#ifdef _WINDOWS // macht in der Firmware kein Sinn da nur mit Multiscannerprojekten geht
#include <zfsfilter/filterapi.h>

#include <string>
#include <vector>
#include <utility>
#include <cstdint>

#include <zfs/Math/zfvec.h>



namespace zfsfilter
{
    class FILTERDLL_API ScanPosRemoverParam
    {
    public:
        double cylAbove{ 0.5 }; ///< [meter]
        double cylBelow{ 2 }; ///< [meter]
        double cylRadius{ 1 }; ///< [meter]
        double maxRange{30}; ///< [meter]
        bool keepGround{ true };
    };

    class FILTERDLL_API ScanPosRemoverResult
    {
    public:
        bool succ{ false };
        uint32_t pointsRemoved{};

        ScanPosRemoverResult() = default;
        ScanPosRemoverResult(bool ok,uint32_t n) : succ(ok), pointsRemoved(n) {}
    };

    /**
    \brief remove scan stativ
    \return success,points
    */
    FILTERDLL_API ScanPosRemoverResult ScanPositionRemover( const std::wstring& zfs, const ScanPosRemoverParam& prm, const std::vector<zfs::math::Vec3d>& pos2remove );
} // end namespace

#endif // _WINDOWS
