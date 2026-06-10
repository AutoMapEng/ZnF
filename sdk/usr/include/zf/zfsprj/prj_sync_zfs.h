#pragma once
#include "Project.h"

#include "xml.h"

#ifdef _WINDOWS
#include <zfs/zfs.h>
#include <zfs/level/calclevel.h>
#else
#include "../zfs/zfs.h"
#include "../zfs/level/calclevel.h"
#endif

#include "config.h"

#include <string>
#include <map>

class ZFProject;

/**
\file prj_sync_zfs.h
@brief Synchronisation
       Daten werden aus den zfs files extrahiert und in das Projektfile uebernommen oder umgekehrt
       z.B Kompensator, Sensoren
       Nur wenn die Daten im Projekt nicht vorhanden, wird der zfs Scan geoeffnet
*/

namespace zfprj
{
    /**
     * Hilfsklasse zum Auswerten des Synchronisations-Vorgangs.
     */
    class ZFSPRJ_API SynchronisationResult {
    public:
        int updates{};
        int status{-1};

        std::vector<QString> message;

        SynchronisationResult() = default;
    };

    //=========================================================================================================

    /**
     * Basisklasse zum Definieren von Handlern die fuer das Synchronisieren von Projekten und Scans benutzt wird.
     *
     * Instanzen werden vom ProjectSynchronizer aufgerufen, wenn Scans mit Projekten synchronisiert werden und vice versa.
     * Hierfuer muessen die Handler beim ProjectSynchronizer registriert werden. Dies erfolgt aktuell in der
     * init()-Methode des ProjectSynchronizer.
     */
    class ZFSPRJ_API SynchronisationHandler {
    protected:

    public:
        SynchronisationHandler() {}
        virtual ~SynchronisationHandler() {}

    public:
        /**
         * Methode wird vom ProjectSynchronizer aufgerufen, wenn ein Scan mit einem Projekt synchronisiert wird.
         * Hier muss das Projekt entsprechend geaendert und ergaenzt werden.
         *
         * @param zfs		der Name des Scans
         * @param header	der Scan-Header
         * @param project	das Projekt
         * @param result	Container zum Speichern der Synchronisations-Nachrichten
         * @parem force		Flag, ob auf jeden Fall eine Synchronisation durchgefuehrt werden soll, auch wenn die Bereiche bereits vorhanden sind
         *
         * @return	true, falls die Synchronisation erfolgreich war, andernfalls false
         */
        virtual bool synchronizeScanToProject(
            const std::wstring& /*zfs*/,
            ZFSHeader& /*header*/,
            ZFProject& /*proj*/,
            SynchronisationResult* /*result = 0*/,
            bool /*force = false*/)
        {
            return true;
        }

        /**
         * Methode wird vom ProjectSynchronizer aufgerufen, wenn ein Projekt mit einem Scan synchronisiert wird.
         * Hier muss der Scan entsprechend geaendert und ergaenzt werden.
         *
         * @param zfs		der Name des Scans
         * @param header	der Scan-Header
         * @param project	das Projekt
         * @param result	Container zum Speichern der Synchronisations-Nachrichten
         * @parem force		Flag, ob auf jeden Fall eine Synchronisation durchgefuehrt werden soll, auch wenn die Bereiche bereits vorhanden sind
         *
         * @return	true, falls die Synchronisation erfolgreich war, andernfalls false
         */
        virtual bool synchronizeProjectToScan(
            const std::wstring&,
            ZFSHeader&,
            ZFProject&,
            SynchronisationResult*,
            bool)
        {
            return true;
        }
    };

    //=========================================================================================================
    /**
    * SynchronisqationHandler fuer IMV Werte
    */

    class ZFSPRJ_API IMUSyncronizer : public SynchronisationHandler
    {
        double angelX =0.;
        double angleY = 0.;
    public:
        IMUSyncronizer() {}

        /**
         * Methode wird vom ProjectSynchronizer aufgerufen, wenn ein Scan mit einem Projekt synchronisiert wird.
         * Hier muss das Projekt entsprechend geaendert und ergaenzt werden.
         *
         * @param zfs		der Name des Scans
         * @param header	der Scan-Header
         * @param project	das Projekt
         * @param result	Container zum Speichern der Synchronisations-Nachrichten
         * @parem force		Flag, ob auf jeden Fall eine Synchronisation durchgefuehrt werden soll, auch wenn die Bereiche bereits vorhanden sind
         *
         * @return	true, falls die Synchronisation erfolgreich war, andernfalls false
         */
        bool synchronizeScanToProject(
            const std::wstring& zfs,
            ZFSHeader& header,
            ZFProject& proj,
            SynchronisationResult* result = 0,
            bool force = false ) override;

    };

    //=========================================================================================================
#ifndef FIRMWARE_BUILD
#define    ERR_KOMPENSATOR_PEAK         1120    // zu schnelle Bewegung
#define    ERR_KOMPENSATOR_OUT          1121    // ausserhalb des Messbereiches
#define    ERR_KOMPENSATOR_NOVALUES     1122    // nur Nullen
#endif

/**
 * SynchronisationHandler fuer Synchronisation der Compensator-Daten.
 */
    class ZFSPRJ_API CompensatorSynchronizer : public SynchronisationHandler
    {
    private:
        double maxTiltAngle{0.5};             // "compensator_max_tilt_angle" = 0.5
        double maxMovementTilt{ 0.007 };          // "compensator_max_movement_tilt" = 0.007

        int num_fastChanges{};
        int num_outOfRange{};

        SynchronisationResult* synchronisationResult{};

        bool checkCompensatorInPrj(const std::wstring& zfs, const ZFProject& project) const;
        bool readCompensator(const std::wstring& zfs, double *m, double *level_x, zf::FileName fn, double *levelmatrix);
        bool checkCompensatorData(const std::wstring& zfs, ZFSLinebyline& lbl);
        bool dynamicCheck(double dyn_lim, int max_num_fastChanges, const zfs::level::levelParams::level_vector_t& x_raw, const zfs::level::levelParams::level_vector_t& y_raw);

    public:
        CompensatorSynchronizer() = default;

        virtual bool synchronizeScanToProject(
            const std::wstring& zfs,
            ZFSHeader& h,
            ZFProject& project,
            SynchronisationResult* result = 0,
            bool force = false);

        virtual bool synchronizeProjectToScan(
            const std::wstring&,
            ZFSHeader&,
            ZFProject&,
            SynchronisationResult*,
            bool)
        {
            return true;
        }

        int getNumberOfFastChanges() const ;
        int getNumberOfRangeErrors() const;

        void setCompensatorLimits(double angle, double movement) { maxTiltAngle = angle; maxMovementTilt = movement; }
    };

    //=========================================================================================================

    /**
     * SynchronisationHandler fuer Synchronisation der Navigations-Daten.
     */
    class ZFSPRJ_API NavigationSynchronizer : public SynchronisationHandler
    {
    private:

    public:
        virtual bool synchronizeScanToProject(
            const std::wstring& zfs,
            ZFSHeader& h,
            ZFProject& project,
            SynchronisationResult* result = 0,
            bool force = false);

        virtual bool synchronizeProjectToScan(
            const std::wstring& zfs,
            ZFSHeader& h,
            ZFProject& project,
            SynchronisationResult* result = 0,
            bool force = false);
    };

    //=========================================================================================================

    /**
    * SynchronisationHandler fuer Scan-Metadaten
    */
    class ZFSPRJ_API MetadataSynchronizer : public SynchronisationHandler
    {
    private:

    public:
        virtual bool synchronizeScanToProject(
            const std::wstring& zfs,
            ZFSHeader& h,
            ZFProject& project,
            SynchronisationResult* result = 0,
            bool force = false);

        virtual bool synchronizeProjectToScan(
            const std::wstring& zfs,
            ZFSHeader& h,
            ZFProject& project,
            SynchronisationResult* result = 0,
            bool force = false);
    };

    //=========================================================================================================

    /**
    * SynchronisationHandler fuer Child-Scans
    */
    class ZFSPRJ_API ChildScanSynchronizer : public SynchronisationHandler
    {
    private:

    public:
        virtual bool synchronizeScanToProject(
            const std::wstring& zfs,
            ZFSHeader& h,
            ZFProject& project,
            SynchronisationResult* result = 0,
            bool force = false);

        virtual bool synchronizeProjectToScan(
            const std::wstring& zfs,
            ZFSHeader& h,
            ZFProject& project,
            SynchronisationResult* result = 0,
            bool force = false);
    };

    //=========================================================================================================

    /**
    * SynchronisationHandler fuer Labels
    */
    class ZFSPRJ_API LabelSynchronizer : public SynchronisationHandler
    {
    private:

    public:
        virtual bool synchronizeScanToProject(
            const std::wstring& zfs,
            ZFSHeader& h,
            ZFProject& project,
            SynchronisationResult* result = 0,
            bool force = false);

        virtual bool synchronizeProjectToScan(
            const std::wstring& zfs,
            ZFSHeader& h,
            ZFProject& project,
            SynchronisationResult* result = 0,
            bool force = false);

        bool findLabel(std::list<ZFVec3d> labelList, ZFVec3d labelPos);

    };

    //=========================================================================================================

    /**
     * SynchronisationHandler fuer Targets.
     */
    class ZFSPRJ_API TargetSynchronizer : public SynchronisationHandler
    {
    private:

    public:
        virtual bool synchronizeScanToProject(
            const std::wstring& zfs,
            ZFSHeader& h,
            ZFProject& project,
            SynchronisationResult* result = 0,
            bool force = false);

        virtual bool synchronizeProjectToScan(
            const std::wstring& zfs,
            ZFSHeader& h,
            ZFProject& project,
            SynchronisationResult* result = 0,
            bool force = false);
    };

    //=========================================================================================================

    /**
     * SynchronisationHandler fuer Synchronisation der Register-Daten.
     *
     * Synchronisierte Daten:
     * Registriermatrix(wenn nicht vorhanden Preregistriermatrix)
     * Scanositionsname
     * Flag fuer die Registriermethode
     *
     *
     * Notizen zur Implementierung
     *
     * Die Funktion synchronizeScanToProject() funktioniert bisher nur, wenn alle noetigen
     * subknoten in der XML-Baumstruktur schon vorhanden sind. Wenn sie die noetigen Subknoten
     * selber einfuegen soll wenn diese noch nicht vorhanden sind, dann muesste viel code aus der
     * Funktion ZFRegisterObjNode::Store() kopiert werden. Um Codeduplikation zu vermeiden muesste
     * man den Code in eine eigene Klass z.B. ProjectXMLTreeManipulator extrahieren, die ueber die
     * Gestallt der Baumstruktur bescheid weiss und die hoehere Funktionen zum erstellen von Knoten
     * zur Verfuegung stellen wuerde.
     * Habe aber im Moment keine Zeit das zu machen.
     *
     */
    class ZFSPRJ_API RegisterDataSynchronizer : public SynchronisationHandler
    {
    private:
         tinyxml2::XMLNode* projectRoot;
        ZFSHeader* header;
        std::wstring shortScanName;

    public:
        RegisterDataSynchronizer();

        /**
        If the scan position names in the scan and project are different, calling this function
        will create a new scan position in the project with the data in the scan and remove the scan
        from its previous position.
        */
        virtual bool synchronizeScanToProject(
            const std::wstring& zfs,
            ZFSHeader& h,
            ZFProject& project,
            SynchronisationResult* result = 0,
            bool force = false);  ///< not implemented

        virtual bool synchronizeProjectToScan(
            const std::wstring& zfs,
            ZFSHeader& h,
            ZFProject& project,
            SynchronisationResult* result = 0,
            bool force = false);

    private:
        // misc
        std::wstring getShortFilename(std::wstring filename);
         tinyxml2::XMLNode* findSubNodeWithTypeAndName( tinyxml2::XMLNode* root, const char* nodeType, std::wstring name);
        // tinyxml2::XMLNode* findTextNodeWithTypeAndText(  tinyxml2::XMLNode* root, const char* nodeType, std::wstring text);
        bool scanPositionNameInHeaderAndProjectAreDifferent();
        bool registrationMatrixesInHeaderAndProjectAreDifferent();
        bool registrationMethodFlagInHeaderAndProjectAreDifferent();
        //bool getRegistrationMatrixFromProjectViewpoint( std::wstring scanPositionName, ZFTRMatrix& matrix);    ///< returns false if there was no viewpoint with the given name.

        // read/write to project xml node
        std::wstring getScanPositionNameForScanFromProject();
        //void setScanPositionNameInProject( std::wstring name);
        ZFTRMatrix getRegistrationOrPreregistrationMatrixFromProjectScanPosition();
        //void setRegistrationMatrixInProject( ZFTRMatrix matrix);
        int getRegistrationMethodFromProjectScanPosition();
        //void setRegistrationMethodInProject(int flag);

        // read/write to ZFSHeader
        ZFTRMatrix getRegistrationMatrixFromHeader();
        void setRegistrationMatrixInHeader(ZFTRMatrix matrix);
        int getRegistrationMethodFromHeader();
        void setRegistrationMethodInHeader(int registrationMethod);
        std::wstring getScanPositionNameFromHeader();
        void setScanPositionNameInHeader(std::wstring positionName);
    };

    //=========================================================================================================

    /**
     * Flags, welche SynchronisationHandler aufgerufen werden sollen.
     */
    enum SYNCHRONIZATION_FLAG
    {
        SYNC_ALL = (1 << 0),	        // Spezialflag, um alle Bereiche zu synchronisieren
        SYNC_REGISTER = (1 << 1),	    // TODO
        SYNC_PREREGISTER = (1 << 2),	// TODO
        SYNC_TARGETS = (1 << 3),	    // TargetSynchronizer wird aufgerufen
        SYNC_LINKS = (1 << 4),	        // TODO
        SYNC_LABELS = (1 << 5),	        // LabelSynchronizer wird aufgerufen
        SYNC_COMPENSATOR = (1 << 6),	// CompensatorSynchronizer wird aufgerufen
        SYNC_NAVIGATION = (1 << 7),	    // NavigationSynchronizer wird aufgerufen
        SYNC_METADATA = (1 << 8),     	// MetadataSynchronizer wird aufgerufen
        SYNC_CHILDS = (1 << 9),       	// ChildScanSynchronizer wird aufgerufen
        SYNC_IMU = ( 1 << 10 )       	// IMUSyncronizer wird aufgerufen
    };

    //=========================================================================================================

    class ZFSPRJ_API ProjectSynchronizer
    {
    private:

        typedef std::map<SYNCHRONIZATION_FLAG, SynchronisationHandler* > HandlerMap;

        /**
         * @brief Enthaelt die registrierten SynchronisationHandler
         */
        HandlerMap handlerMap;

        /**
         * @brief Initialisiert ProjectSynchronizer.
         *
         * Hier werden die SynchronisationHandler registriert.
         */
        void init();

    public:
        /**
         * @brief Flag, ob der ProjectSynchronizer bereits initialisiert wurde.
         */
        bool isInit;

        /**
         * @brief CTOR
         */
        ProjectSynchronizer() : isInit(false) { }
        ~ProjectSynchronizer();

        /**
         * Methode startet die Synchronisation eines Projekts mit einem Scan.
         *
         * @param project	das Projekt, aus dem die Aenderungden uebernommen werden sollen
         * @param zfs		der Scan-Name, in den die Aenderungen geschrieben werden sollen
         * @param mask		Flag, welche Synchronisationsschritte gemacht werden sollen
         * @param result	Container zum Speichern der Synchronisations-Nachrichten
         * @parem force		Flag, ob auf jeden Fall eine Synchronisation durchgefuehrt werden soll, auch wenn die Bereiche bereits vorhanden sind
         *
         * @return	true, falls die Synchronisation erfolgreich war, andernfalls false
         */
        bool synchronizeProjectToScan(
            ZFProject& project,
            const std::wstring& zfs,
            int mask,
            bool save = true,
            SynchronisationResult* result = 0,
            bool force = false);

        /**
         * Methode startet die Synchronisation eines Scans mit Projektdaten. Falls die Synchronisation erfolgreich war
         * wird das Projektfile gespeichert.
         *
         * @param zfs		der Scan-Name des Scans, der geaendert werden soll
         * @param project	das Projekt, in das die Daten uebernommen werden sollen
         * @param mask		Flag, welche Synchronisationsschritte gemacht werden sollen
         * @param result	Container zum Speichern der Synchronisations-Nachrichten
         * @parem force		Flag, ob auf jeden Fall eine Synchronisation durchgefuehrt werden soll, auch wenn die Bereiche bereits vorhanden sind
         *
         * @return	true, falls die Synchronisation erfolgreich war, andernfalls false
         */
        bool synchronizeScanToProject(
            const std::wstring& zfs,
            ZFProject& project,
            int mask,
            bool save = true,
            SynchronisationResult* result = 0,
            bool force = false);
    };
} // end namespace
