#pragma once

#include <exception>
#include <string>

#include "ZfsPrjBasic.h"

// errorcodes while register

class QString;

enum regerr_t
{
    REGERR_UNKNOWN = -1,           ///< Unbekannter Fehler
    REGERR_OK = 0,            ///< kein fehler

    REGERR_FAILED = 1,            ///< unspezified error
    REGERR_NO_OPEN_PROJ = 2,            ///< kein projekt offen

    REGERR_UPDATEPRJ = 10,           ///<
    REGERR_UPDATEPRJ_UNKNOWN = 11,           ///< Fehler wurde im catch() Block aufgefangen

    REGERR_CANT_START_SCANTRA = 20,           ///< Scantra konnte nicht gestartet werden.
    REGERR_CANT_READ_RESULT = 21,           ///< Eine oder mehrere der Ergebnisdateien konnte nicht gelesen werden.

    REGERR_TACHY_LOAD = 101,          ///< fehler beim laden / oeffnen des tachy files
    REGERR_TACHY_FORMAT = 102,          ///< unbekanntes dateiformat des tachy files
    REGERR_NOT_ENAUGH_TARGETS = 103,

    REGERR_STANDPUNKT_LOAD = 201,          ///<
    REGERR_STANDPUNKT_FORMAT = 202,          ///<
    REGERR_STANDPUNKT_NO2 = 203,          ///< keine gegenueberliegende punkte

    REGERR_LESSTARGETS = 300,          ///< zu wenig targets

    REGERR_POS_DIST = 400,          ///< 

    REGERR_WR_ZFS_HEADER = 500,          ///< fehler beim schreiben des zfs headers

    REGERR_GPS_MASTER = 600,          ///< 

    REGERR_WRITE = 700,          ///<
    ////textausgabe
    REGERR_OPEN_SCAN = 800,          ///< scan konnte nicht geoeffnet werden
    REGERR_NO_ZFS = 810,          ///< es wurde kein ZFScan objekt uebergeben (interner Fehler)
    REGERR_CANT_CALCULATE = 820,          ///< Registrierung konnte nicht berechnet werden
    REGERR_NOT_ALL_SCANS_CALCULATED = 821,
    REGERR_NOT_ENAUGH_SPACE = 822,
    
    REGERR_NO_MASTER_SCAN = 830,
    REGERR_NOTHING_TO_DO = 840,
    
    REGERR_NO_SCANS = 850,
    REGERR_NO_ZFDB = 851,
    REGERR_NO_ZFDB_LOD = 852,
    REGERR_NO_VALID_SCANS = 581,
    REGERR_USER_ABORT = 860,
    REGERR_NO_REGISTERED_TARGETS = 870,
    REGERR_NO_SP_TARGETS = 880,
    REGERR_CANT_FIND_SCANPOS = 890,
    REGERR_TO_LES_PIXEL = 900,
    REGERR_UNREGISTERED_SCANPOS = 910,  //!< es gibt noch unregistierte scanpositionen
    REGERR_NOTHING_FOUND = 920,         //!< Keine Connections fuer die Scanposition gefunden

    REGERR_C2C_NOTCONSTRAINT        = 1000,

    REGERR_CANT_WRITE_SCANTRA_DB    = 1010,     //!< Scantra Datenbank kann nicht geschrieben werden.
    REGERR_EXIT_SCANTRA             = 1011,     //!< Scantra kann nicht gestartet werden.
    REGERR_CANT_READ_SCANTRA_DB     = 1012,     //!< Scantra Datenbank kann nicht geoeffnet werden.
    REGERR_CANT_FIND_STATION        = 1013,     //!< Beim Umbennen wurde der Standpunkt nicht gefunden
    REGERR_CANT_OPEN_SCANTRAAPI     = 1014,     //!< Scantra API Dll kann nicht geladen werden

    REGERR_NOT_SUPORTED             = 1100,

    REGERR_                                        ///< max Nr
};


ZFSPRJ_API QString RegErrorString(regerr_t e);


namespace registermodul
{
    /**
    * Exception klasse für LRCServer Error codes.
    *
    * Kann im neuem Code geworfen werden, statt den Error-Code zurück zu
    * geben.
    */
    class RegisterError : public std::exception 
    {
    public:

        /**
        * Konstruiert einen neuen RegisterError
        *
        * @param code Fehlercode der diesen Fehler ausloest
        * @param info  Zusaetzliche Beschreibung des Fehlers
        */
        RegisterError(regerr_t code = REGERR_OK, const std::wstring& info = L"")
            : m_code(code)
        {
            if (!info.empty())
                m_info = info;
        }

        /// return errorcode
        regerr_t code() const { return m_code; }

        /// return additional info
        const wchar_t *info() const { return m_info.c_str(); }

        RegisterError& operator=(const RegisterError&) { return *this; }

    private:
        const regerr_t		m_code;		///< errorcode
        std::wstring		m_info;		///< weitere infos zu dem fehler
    };


} // end namespace
