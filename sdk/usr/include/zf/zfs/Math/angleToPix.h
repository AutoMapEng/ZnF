#pragma once

#include <vector>

#include "../zfsapi.h"


class ZFScanZFS;
class ZFSLinebyline;
class ZFTheoModel;


/**
\brief calculate pixel and line back from a 3D value
*/
class ZFS_API ZFAngleToPixel
{
public:
    enum layer_t {
        ISOUT,      ///< failed
        ISFRONT,    ///< 0..180deg
        ISBACK      ///< 180..360deg
    };

    ZFAngleToPixel();
    virtual ~ZFAngleToPixel();

    /// init and create LUT
    bool calc(ZFScanZFS* zfs);
    bool calc(ZFSLinebyline* zfs);

    bool angleToPixel(double uncalHenc, double uncalLenc, int &u, int &v, bool forceScdHalf=false) const;
    bool angleToPixel(double uncalHenc, double uncalLenc, double &u, double &v, bool forceScdHalf = false) const; // obsolete
    layer_t getLayer(double uncalHenc, double uncalLenc) const;

private:
    ZFScanZFS* zfs{};
    ZFSLinebyline* zflbl{};
    int NP{}, NL{};                     ///< anzahl pixel,zeilen
    std::vector<double> LUT_AzimEncToPix_mRad;
    std::vector<double> LUT_ElevEncToPix_mRad;
};

//#####################################################################################################################################

ZFS_API bool xyz2Pix(double x, double y, double z, const ZFAngleToPixel &a2p, const ZFTheoModel *theo, double &pixel, double &line, bool forceScdLayer, double eleOffset );
ZFS_API bool xyz2Pix_calib(double x, double y, double z, const ZFAngleToPixel &a2p, const ZFTheoModel *theo, double &pixel, double &line, bool forceScdLayer, double eleOffset);
ZFS_API bool xyz2PixProfiler(double x, double y, double z, const ZFAngleToPixel &a2p, const ZFTheoModel *theo, double xAxis, double &pixel, double &line, int linezfs, double eleOffset);
