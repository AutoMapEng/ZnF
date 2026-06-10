#pragma once

#include "ZfsPrjBasic.h"

#include <string>

class ZFProject;
namespace zfprj
{

    /**
    \brief holds info for sun position
    */
    class ZFSPRJ_API SunKoordinaten
    {
    public:
        double azimutNorth;
        double azimutScan0;     ///< when scan has north orientation
        double hoehenwinkel;    ///< elevation

    public:
        SunKoordinaten() : azimutNorth(0.), azimutScan0(0.),hoehenwinkel(0.) {}

        bool isValid() const { return azimutScan0 || hoehenwinkel; }
    };


    /**
    *   Gibt die Position der Sonne fuer den Scan zurueckt.
    */
    ZFSPRJ_API SunKoordinaten getSunPositonFromScan( ZFProject&, const std::wstring& scanName);

    /// global flag whether sun position is suppoorted
    ZFSPRJ_API bool sunPositonSupported();
    ZFSPRJ_API void setSunPositonSupported( bool );
}

