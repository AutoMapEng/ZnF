#ifndef __ZFSPRJ_TARGET_TOOLS_H__
#define __ZFSPRJ_TARGET_TOOLS_H__

#include "ZfsPrjBasic.h"

#include <string>

#include "target.h"

#include "../zfs/app.h"

class QImage;
class ZFProject;
class ZFSHeader;
class ZFScan;
class ZFSLinebyline;
class ZFProjectItemScan;


namespace zftarget
{


    //Target

    enum {
        FLTARGTE_UPDATEPRJ = 1,
        FLTARGET_ZFSASWELL = 2,
        FLTARGET_UPDATESIGNAL = 4,
        FLTARGET_WRITE_HEADER = 8
    };

    //ZFScan Objekt bearbeiten

    /**
    *	Liest alle Targets ein und speichert sie als Objecte in der ZFScan Klasse.
    */
    ZFSPRJ_API int TargetsReadZFSObject(ZFScan* zfScan);

    /**
    *	Update die Bilder zu allen Targets des Projektes. Falls das Bild nicht vorhanden ist wird es angelegt. Vorhandene
    *	Bilder werden nicht angefasst.
    *	\param prj => Projekt das upgedated werden soll.
    *   \return number of images created
    */
    ZFSPRJ_API int updateTargetImageInProject(ZFProject& prj, zfs::ProgressCallbackInterface* callback = nullptr);


    //Allgemeine Sachten
    /**
        Dateiname des Targetbildes im .temp Ordner
    */
    ZFSPRJ_API std::wstring BuildTargetFilename4Image(const std::wstring& zfs, const std::wstring& name);

    /**
    *	Erstellt ein kleines Bild von dem Punkt und seiner Umgebung
    *	Wenn ein Scan beim erstellen des Targets offen ist sollte auch damit das Bild gemacht werden sonst wird es
    *	beim Speichern des Targets im Header erstellt. Dabei muss der Scan dann wieder geoeffnet werden.
    *	\param img => Reuckgabevairable fuer das Bild
    *	\param z => LineByliylen aus der Das Bild berechnet wird.
    *	\param u => Pixel Position des Mittelpunktes in Scankoordinaten
    *	\param v => Line Position des Mittelpunktes in Scankoordinaten
    *	\param w => Breite und Hoehe des Bildes muss quadratisch sein um bei der umrechnung in bildkoordinaten nicht zu unerwuenschten nebenwirkungen zu fuehren.
    */
    ZFSPRJ_API bool TargetImageFromPos(QImage& img, ZFSLinebyline* z, int u, int v, int w = 150, bool cross=true);
    ZFSPRJ_API bool imageFromPos(QImage& img, ZFSLinebyline* z, int u, int v, int w, int h, bool cross, bool resize = true);

    /**
    *	Siehe TargetImageFromPos( QImage& img, ZFSLinebyline* z, int u, int v, int w )
    *	\param u => Pixel Position des Mittelpunktes in Bildkoordinaten
    *	\param v => Line Position des Mittelpunktes in Bildkoordinaten
    */
    ZFSPRJ_API bool TargetImageFromPos(QImage& img, ZFScan* z, int u, int v, int w = 150, bool cross = true );
    ZFSPRJ_API bool imageFromPos(QImage& img, ZFScan* z, int u, int v, int w, int h, bool cross );

    ZFSPRJ_API const std::wstring getTargetInfo( const ZFTarget& target, const ZFTRMatrix& world);

    ZFSPRJ_API ZFTarget findTargetScan(
        ZFProject& prj,
        const std::wstring& scan,
        const std::wstring& targetName);

    /**
    @brief Alle Targets aufn ? setzen mit durchlaufender Nummerierung. Vor allem fuer Testzwecke. Zuordnung geht verlohren
    @return anzahl der Targets die umbenannt wurden
    */
    ZFSPRJ_API int TargetsSet2Unknown(ZFProject&);

    /**
    *   Aus allen scans im Projekt die Targets aus den Headern in das Projekt uebernehmen.
    *   \param project => Projekt das upgedated werden soll.
    */
    ZFSPRJ_API void UpdateTargetsFromScanHeaders( ZFProject& project );

}

#endif //__ZFSPRJ_TARGET_TOOLS_H__
