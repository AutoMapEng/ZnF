#pragma once

#include "../zfsapi.h"

#include <string>

namespace zfs
{
    /**
    * ScanFileCopyHelper
    * Modifizierte Scandateien sollen im Project active sein. Scan muss ersetzt werden
    * Orginalscan wird im Verzeichniss .orginal abgelegt
    * Mehrfachaufruf laesst das Orginal
    */
    class ZFS_API ScanFileCopyHelper
    {
    public:
        /**
        * @param info = string der gespeichert wird um zu sehen was alles gefiltert wurde
        */
        ScanFileCopyHelper();
        ScanFileCopyHelper( const std::wstring& a_ScanDerMofifiziertWerdenSoll,
                            const std::wstring& a_Info = L"",
                            bool a_bCopyAtDestruction = true,
                            bool a_bDiscardOriginals= false );

        ~ScanFileCopyHelper();

        /// initialization function (for use within init function(s) in other classes)
        void init( const std::wstring& a_ScanDerMofifiziertWerdenSoll, 
                   const std::wstring& a_Info = L"", 
                   bool a_bCopyAtDestruction  = true,
                   bool a_bDiscardOriginals   = false);

        void setTempFileName(const std::wstring& aTempFileName); // possibility to modify

        /// temp file erzeugen. In diese Datei muss das Ergebniss gespeichert werden
        std::wstring tempFile() const;

        /// funktion wurde abgebrochen. Nichts wird gespeichert. Alles bleibt wie es ist
        bool abort();

        /// Ende des processes. orginal wird mit temp ersetzt. orginal wird im subfolder .orginal gespeichert falls noch nicht geschehen
        bool fin(const bool& bDiscardOriginal = false);

        /// subfolder in dem die orginal Kopien abgelegt werden
        static const wchar_t* orgKopieSubFolder();

        /// original scan pfad
        std::wstring Scan() const;

        // helper for part files
        std::wstring tempPartFileSplitted(int n) const;
        std::wstring PartFile(int n) const;

    protected:
        bool         m_bCopyAtDestruction;              ///< allow to switch off copy at destruction (maybe remove in later/enhanced versions)
        bool         m_bDiscardOriginal;                ///< indicates if originals are to be discarded
        std::wstring m_ScanDerModifiziertWerdenSoll;	///< orginal datei (absoluter path)
        std::wstring m_Info;							///< info fuer report
        std::wstring m_OrgKopiePath;					///< der path in dem die orginaldateien gespeichert werden
        std::wstring m_Temp;							///< absoluter Dateiname der temp Datei

    private:
        void basic_init();


    public:
        friend void ZFS_API HandlePartFiles( const ScanFileCopyHelper& a_ScanFileCopyHelper );
        friend void ZFS_API HandleAbortForPartFiles( const ScanFileCopyHelper& a_ScanFileCopyHelper );

    };
    // ===================================================================
    void ZFS_API HandlePartFiles( const ScanFileCopyHelper& src );
    // ===================================================================
    void ZFS_API HandleAbortForPartFiles( const ScanFileCopyHelper& src );
    // ===================================================================

} // end namespace
