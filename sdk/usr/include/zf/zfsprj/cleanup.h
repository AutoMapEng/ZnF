#pragma once

/**
@brief	cleanup
        Einzelne Bereiche oder Dateien loeschen
*/

#include "ZfsPrjBasic.h"

#include <string>

class ZFProject;
class ZFScan;
class ZFSHeader;

namespace zfprj
{
    class ZFSPRJ_API CleanupParams
    {
    public:
        bool temp {};				///< .temp verzeichniss loeschen
        bool regTemp {};			///< .temp verzeichniss loeschen
        bool downloadFragments {};  ///< Download-Fragmente löschen
        bool reg {};				///<registrierung loeschen
        bool delRegFlag {};			///<Den Status der Registrierung auf Manuell setzten die Matrix wird nicht veraendert
        bool aktivateConnections {};///< Alle Connections werden aktiviert
        bool prjreg {};
        bool masks {};				///< masken in den scans loeschen
        bool targets {};			///< Targets loeschen
        bool slamTargets{};         ///< Slam Targets loeschen
        bool links {};
        bool linkFiles {};
        bool color {};				///< everything with color. Use with care
        bool colorGeneratedOutput {}; ///< nur die _color dateien loeschen
        bool colorQuality {};		///< die qualitaetsbilder im unterverzeichniss overlay
        bool colorPanorama {};		///< highres color panorama output
        bool colorAdj {};           ///< adjusted color
        bool tcamOutput {};		    ///< ergebniss dateien die beim mappen erzeugt werden
        bool camera {};
		bool cameraCalibZfi {};	    ////< remove the camera from zfi container
        bool exp {};				///< export folder loeschen
        bool mess {};				///< messungen loeschen
        bool layers {};
        bool planes {};
        bool views {};
        bool shoot {};
        bool regConnections {};	    ///< verbindungen loeschen
        bool topCut {};			    ///< topcut im .temp verzeichniss loeschen
        bool scantra {};			///< Scantra Verzeichnis loeschen
        bool santraScout {};		///< ScantraScout check info aus dem projekt loeschen
        bool knownTargets {};		///< bekannte targets aus dem projekt entfernen
        bool navigation {};		    ///< Navigationsknoten aus Projekt-Datei loeschen
        bool compensator {};		///< Compensatorknoten aus Projekt-Datei loeschen
        bool filesAutoplacement {}; ///< dateien die fuer das placement von vr benoetigt werden
        bool targetsRandomNames {}; ///< targets durchlaufend durchnummerieren mit ? am anfang. Es besteht keine Targetzuordung der einzelnen Scans mehr
        bool timeApi {};            ///< cleanup the .time files
        bool deletedScan {};        ///< delete scan entries in project for deleted scans
        bool deletedScanFiles {};   ///< delete scan files for deleted scans
        bool deleteEmtyScans{};     ///< delete the scan entry if the scan is not available
        bool zfdb {};               ///< delete point cloud database
        bool labels{};              ///< delete labelling results generated using AI

        CleanupParams()
        {
            Set( false );
            targetsRandomNames = false;
            regTemp = false;
        }

        void Set( bool en )
        {
            temp =
            regTemp =
            downloadFragments =
            reg =
            delRegFlag=
            aktivateConnections=
            prjreg =
            masks =
            targets =
            slamTargets =
            links =
            linkFiles =
            color =
            colorGeneratedOutput =
            colorQuality =
            colorPanorama =
            colorAdj =
            tcamOutput =
            camera =
			cameraCalibZfi =
            exp =
            mess =
            layers =
            planes =
            views  =
            shoot =
            regConnections =
            topCut =
            scantra =
            santraScout =
            knownTargets =
            navigation =
            compensator =
            filesAutoplacement =
            targetsRandomNames =
            timeApi =
            deletedScanFiles =
            zfdb =
            labels =
                en;
        }

        void All()
        {
            Set(true);
            color = false;
            regTemp = false;
        }

        /// parse params from a string like '-mask -reg'
        void parseParamsFromString(const std::string& str);
    };


    /// cleanup project and scans depending on params. returns string of items which are cleaned up
    ZFSPRJ_API std::string Cleanup( const std::wstring& prjFileName, CleanupParams& p, std::wstring& errorString );
    /// same as bevore but potinter to project
    ZFSPRJ_API std::string Cleanup( ZFProject&, CleanupParams& p, std::wstring& errorString );

    /// cleanup scan masks
    ZFSPRJ_API void CleanupMasks( ZFScan* zfs );

    /// cleanups for things stored in the zfs-heder
    ZFSPRJ_API void CleanupFromZFSHeader( ZFSHeader* h, bool temp, bool reg, bool labels, bool links, bool color, bool targets );

    ZFSPRJ_API void CleanupSlamTargets(ZFProject& prj, const std::list < std::wstring>& runs);

    ZFSPRJ_API void CleanupSlamLabelling(ZFProject& prj);


} // end namespace
