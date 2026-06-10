#pragma once

#include <zfsfilter/filterapi.h>

#include <string>
#include <utility>
#include <exception>

#ifdef _WINDOWS

class ZFSHeader;

namespace zfsfilter
{
    namespace repair
    {

        // check if index:4 (lines) available
        FILTERDLL_API
            bool check_zfsheader_lines_available(const std::wstring& zfsfilename );
        FILTERDLL_API
            bool check_zfsheader_lines_available(const ZFSHeader*);



        /**
        \brief Repariert scandateien. Der zfs-header muss korrekt vorhanden sein.
               Es wird geschaut wie viele Zeilen vorhanden sind und der Zaehler im zfs-header wird angepasst
        \return first = true if repaired, false if scan ok
                second = new file name for repaired scan
        */
        FILTERDLL_API bool repairCorrupdedLines( const std::wstring& absZfsFileName );

        // same with exceptions
        class RepairCorruptedScanResult
        {
        public:
            bool scanOk{ true };
            bool repaired{ false }; 
            std::wstring repairedScanFileName;
            std::string repairInfo;
        };
        FILTERDLL_API RepairCorruptedScanResult repairCorrupdedLinesX(const std::wstring& absZfsFileName);




        /**
        \brief repair pixel and or line definition in the zfs header
        \return int 0=scan already ok, nothing to do 1=repaired -1=repair failed or not possible
        */
        FILTERDLL_API std::pair<int,std::string> repairPixelAndLines(const std::wstring& absZfsFileName);
    }
}
#endif // _WINDOWS