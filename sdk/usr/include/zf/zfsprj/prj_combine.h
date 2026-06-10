#pragma once
#include "ZfsPrjBasic.h"
#include "config.h"

#include "xml.h"

#pragma warning(push)
#pragma warning(disable: 4610 4512 4510)
#pragma warning(pop)

#include <mutex>
#include <string>
#include <vector>
#include <map>

#include "../zfs/trmatrix.h"

class ZFProject;
class ZFProjectItemScan;

namespace zfprj
{

    //=========================================================================================================

    /**
    * Class to define the update process.
    *
    * It defines the elements which should be included in the update process (e.g. register data, scans etc.)
    *
    * You can specify if a reload of the current project should be avoided if possible. This tries to use instances of
    * a UpdateResultAction to perform the change without reload.
    *
    */
    class UpdateDefinition {

    public:
        /**
        * @brief Available update elements
        */
        enum UPDATE_CASE {	
            PROJECT		            = (1<<0),	///< all
            SCANS			        = (1<<1),	///< Scans only
            REGISTRATION	        = (1<<2),	///< only Registrier Daten
            FORCE_INACCURATE	    = (1<<3),	///< Flag, if changes overrides other changes with inaccurate register data
            FORCE_ALL       	    = (1<<4),	///< Flag, if changes overrides other changes with register data (even fine registerd)
            PROJECT_IGNORE_EXISTING = (1<<5),	///< all but do not update existing VPs and scans (used to merge projects)
            FORCE_GROUPS            = (1<<6),	///< merge the groups as well (used to merge projects)
            FORCE_COORDINATESYSTEMS = (1<<7),	///< Force coordinate systems
            FORCE_DELETED            = (1<<8)	///< Force update of deleted items
        };

        int updateFlags;	///< Flag holding the Update-Definition

        bool avoidReload;   ///< Flag to define, if we try to avoid project reload after merging the projects

        UpdateDefinition() : updateFlags(PROJECT), avoidReload(true) {}

        /**
        * @brief Checks if the element has to be updated
        *
        * @param element  the element name
        *
        * @return true, if the element or a child node has to be included in the update
        */
        bool checkUpdate(const char* element) const;
    };

    //=========================================================================================================

    /**
    * @brief Base class to define update result actions.
    *
    * Implementing classes can be used to reflect changes to the XML project file directly in the open project to
    * avoid unneccessary reloading the hole project.
    *
    * After a project has been synchronised with another project file, each update action is executed to reflect
    * the changes in the current open project.
    */
    class UpdateResultAction {

    public:
        /**
        * @brief List containing all messages collected during an update action
        */
        std::map<QString, QString> actionParameter;
        bool retry;

        UpdateResultAction() : retry(false) {}
        virtual ~UpdateResultAction() {}

        /**
        * @brief This method gets called after the update has been finished.
        *        Here we have to performe all actions to reflect the associated change in an open project.
        */
        virtual bool execute() = 0;

        /**
        * @brief Get an log string for this update action
        */
        virtual QString toString() { return ""; }
    };

    //=========================================================================================================

    /**
    * @brief Update result action for a new scan
    */
    class AddScanAction : public UpdateResultAction {

    public:
        AddScanAction( tinyxml2::XMLElement* scanNode);
        virtual ~AddScanAction() {}

        tinyxml2::XMLDocument doc;
        tinyxml2::XMLElement* scanNode;
        ZFProjectItemScan* scan;

        /**
        * @brief Actions to be taken: Add the defined scan directly
        */
        virtual bool execute();

        virtual QString toString() { return QString("AddScanAction %1").arg(actionParameter["name"]); }
    };

    //=========================================================================================================

    /**
    * @brief Update result action for a new scan
    */
    class AddViewpointAction : public UpdateResultAction {

    public:
        AddViewpointAction( tinyxml2::XMLElement* scanNode, ZFTRMatrix distanceMatrix);
        virtual ~AddViewpointAction() {}

        tinyxml2::XMLDocument doc;
        tinyxml2::XMLElement* viewpointNode;

        /**
        *  @brief delta between the last two scans
        */
        ZFTRMatrix distanceMatrix;
        
        /**
        * @brief Actions to be taken: Add the defined viewpoint directly
        */
        virtual bool execute();

        virtual QString toString() { return QString("AddViewpointAction"); }
    };

    //=========================================================================================================

    /**
    * @brief Update result action for scan attributes
    */
    class ChangeScanAttributeAction : public UpdateResultAction {

    public:
        ChangeScanAttributeAction( tinyxml2::XMLElement* scanNode, const  tinyxml2::XMLAttribute* attribute);
        virtual ~ChangeScanAttributeAction() {}

        /**
        * @brief Actions to be taken: Change the given attribute of the scan
        */
        virtual bool execute();

        virtual QString toString() { return QString("ChangeScanAttributeAction %1").arg(actionParameter["name"]); }
    };
    //=========================================================================================================

    /**
    * @brief Update result action for changed project parameter
    */
    class UpdateProjectParameterAction : public UpdateResultAction {

    public:
        UpdateProjectParameterAction( tinyxml2::XMLElement* viewpointNode);
        virtual ~UpdateProjectParameterAction() {}

        /**
        * @brief Actions to be taken: change the project parameter
        */
        virtual bool execute();

        virtual QString toString() { return QString("UpdateProjectParameterAction %1").arg(actionParameter["paramName"]); }
    };

    //=========================================================================================================

    /**
    * @brief Update result action for changed register data
    */
    class UpdateRegisterDataAction : public UpdateResultAction {

    public:
        UpdateRegisterDataAction( tinyxml2::XMLElement* viewpointNode);
        virtual ~UpdateRegisterDataAction() {}

        /**
        * @brief Actions to be taken: change the register data of the given viewpoint
        */
        virtual bool execute();

        virtual QString toString() { return QString("UpdateRegisterDataAction %1").arg(actionParameter["name"]); }
    };

    //=========================================================================================================

    /**
    * @brief Helper class to store update results. Inserts and updates are counted and the overall result can be set.
    *
    * It contains a list of update actions, which has to be done to reflect the changes in the project file.
    * Some changes in the project can only be reflected by reloading the whole project. This can be achieved by setting the needsProjectReload flag.
    */
    class UpdateResult {

    protected:
        /**
        * @brief List of update actions, which has to be done to reflect the changes in the project file.
        *
        * This can be following actions:
        *
        * <list>
        * <li>AddScanAction
        * </li>
        * <li>AddViewpointAction
        * </li>
        * <li>ChangeScanAttributeAction
        * </li>
        * <li>ChangeRegisterDataAction
        * </li>
        * <li>UpdateProjectParameterAction
        * </li>
        * </list>
        *
        * Check the documentation of the execution method of these actions to get more details about their update work.
        */
        std::vector<UpdateResultAction*> actions;

    public:
        enum UpdateResultFlag {
            UPDATE_UNKNOWN  = 0,
            UPDATE_OK       = 1,
            UPDATE_FAILED   = 2,
            UPDATE_CONFLICT = 4
        };

        ZFSPRJ_API UpdateResult() : updates(0), inserts(0), result(UPDATE_UNKNOWN), needsProjectReload(false)  {}
        ZFSPRJ_API ~UpdateResult();

        /**
        * @brief Counter for node updates.
        */
        int updates;

        /**

        * @brief Counter for node inserts.
        */
        int inserts;

        /**
        * @brief Result-Flag of the update.
        */
        UpdateResultFlag result;

        std::vector<QString> conflictMessage;

        /**
        * @brief List containing all messages collected during an update action
        */
        std::vector<QString> message;

        /**
        * @brief List containing all scans which have a different scan date
        */
        std::vector<QString> scansWithChangedDate;

        /**
        * @brief Flag, if the project has to be reloaded completely. This should be avoided if possible because this
        *        disturbs the user work.
        */
        bool needsProjectReload;

        /** 
        * @brief if we add viewpoints, this is the matrix containing the rotation added
        *        to the last known viewpoint since the last synchronisation
        */
        ZFTRMatrix rotationDistance;

        /**
        * @brief Add a new update action to the result. The ownership is taken.
        *
        * @param action a pointer to the new action. The ownership is taken.
        */
        void addUpdateAction(UpdateResultAction* action);

        /**
        * @brief Get the list of UpdateResultAction objects in this UpdateResult.
        *
        * @return the list of UpdateResultAction objects
        */
        std::vector<UpdateResultAction*>& getUpdateActions() { return actions; }

    };

    //=========================================================================================================

    class ProjectCombiner
    {

    private:
        /**
        * @brief CTOR, Private fuer Singleton
        */
        ProjectCombiner() { }
        ~ProjectCombiner() { }

        std::mutex combineMutex;
        std::string m_log;

    public:
        /**
        * @brief Zugriff auf die Singleton-Instanz de ProjectCombiner-Instanz.
        *
        * @return die Instanz
        */
        ZFSPRJ_API static ProjectCombiner& getInstance();

        /*
        * @brief Merged die beiden angegebenen Projekte.
        *
        * @param masterProject der Dateiname des Master-Projekts
        * @param updateProject der Dateiname des Projekts das hineingemerged werden soll
        * @param definition Definition, welche Bereiche gemerged werden sollen
        * @param updateResult Ergebnis des mergens
        *
        * @return false, falls der Merge fehlgeschlagen ist, ansonsten true
        */
        ZFPRJ_API bool combineProjects(
            const std::wstring& masterProject,
            const std::wstring& updateProject,
            const UpdateDefinition& definition = UpdateDefinition(),
            UpdateResult* result = 0 );

        /*
        * @brief Merged die beiden angegebenen Projekte.
        *
        * @param masterProject das Master-Projekts
        * @param updateProject das Projekts das hineingemerged werden soll
        * @param definition Definition, welche Bereiche gemerged werden sollen
        * @param updateResult Ergebnis des mergens
        *
        * @return false, falls der Merge fehlgeschlagen ist, ansonsten true
        */
        ZFPRJ_API bool combineProjects(
            ZFProject& masterProject,
            ZFProject& updateProject,
            const UpdateDefinition& definition = UpdateDefinition(),
            UpdateResult* result = 0 );

        void resetLog() { m_log.clear(); }
        const std::string& getLog() const { return m_log; }


        /**
        *   Start Scantra to combine the both Scantra projects. 
        *   \param prj1 => master project 
        *   \param prj2 => Projekt to add to the master project.
        *   \param maxWait => Maximum iteration fo wait to exit scantra. One step are 1 sec. Only for tests!
        */
        ZFSPRJ_API bool combineScantraProjects(const std::wstring& prj1, const std::wstring& prj2, int maxWait = -1 );

        /**
        * @brief Merged die beiden Projekt-Knoten. Das Resultat steht am Ende im masterNode.
        *
        * Es werden einige Sonderregeln angewandt:
        * 1.) Listen ohne Namens-Attribut werden erweitert, falls neue Kind-Knoten im Update stehen
        * 2.) beim Register-Element dient REGXML_SCANPOS als Namens-Attribut
        * 3.) Text-Knoten werden ersetzt und nicht ergänzt
        * 4.) Punkt-Listen werden komplett ersetzt
        * 5.) bei zwei unterschiedlichen Projekten werden fuer Viewpoints Gruppen eingefuegt
        * 6.) Jobs-Knoten werden nicht synchronisiert
        *
        * @param masterNode der Master, in den die Aenderungen uebernommen werden
        * @param updateNode der Knoten mit den Neuerungen, die gemerged werden sollen
        * @param defintion	Definition der Update-Schritte
        * @param result		Container fuer die Ergebnisrueckgabe
        */
        ZFPRJ_API void updateXMLNode(  tinyxml2::XMLNode* masterNode,  tinyxml2::XMLNode* updateNode, const UpdateDefinition* definition = NULL, UpdateResult* result = NULL);

        void activateReload(zfprj::UpdateResult * result);

        void checkGroupRef( tinyxml2::XMLElement * updateChildElement,  tinyxml2::XMLNode * masterNode,  tinyxml2::XMLNode * updateNode, zfprj::UpdateResult * result);

        /**
        * @brief Hilfsmethode zum Hinzufuegen einer Gruppe zu einem Projekt. Die Gruppen werden im Root-Knoten verortet.
        *		  Der Eintrag benutzt die Definitionen REGXML_GROUPS und REGXML_GROUP als Elementnamen und hat folgendes Aussehen:
        *
        *			 <Groups>
        *				<Group name='Project1'>
        *				</Group>
        *			</Groups>
        *
        * @param masterProjectDoc Das Document
        * @param masterProjectName der Name des Projects
        *
        * @return true, falls die Gruppen hinzugefuegt wurde, ansonsten false
        */
        bool addGroup( tinyxml2::XMLDocument* masterProjectDoc, const char* masterProjectName);

        /**
        * @brief Hilfsmethode zum Hinzufuegen einer Gruppenreferenz zu einem Viewpoint.
        *		  Der Eintrag benutzt die Definitionen REGXML_GROUPS_REF und REGXML_GROUP_REF als Elementnamen und hat folgendes Aussehen:
        *
        *			<GroupsRef>
        *				<GroupRef>GroupName</GroupRef>
        *			</GroupsRef>
        *
        * @param viewpointNode der Viewpoint-Knoten
        * @param groupName der Name der Gruppe
        *
        * @return true, falls die Gruppenreferenz hinzugefuegt wurde, ansonsten false
        */
        bool addGroupRef( tinyxml2::XMLNode* viewpointNode, const char* groupName);

        /**
        * @brief Hilfsmethode zum Checken, ob das Element eine Liste ist, die kein Namens-Attribut hat.
        * Dieser Check muss erweitert werden, falls neue Listen diesen Typs hinzugefuegt werden.
        *
        * @param elementName der Name des Element-Knotens
        *
        * @return true, falls das Element zu dieser Liste gehoert
        */
        bool isUnnamedList(const char* elementName);

        bool conflictPlanAttribute(const char* attributeName);
        bool updatePlanAttribute(const char* attributeName);

        /**
        * @brief Hilfsmethode zum Checken, ob ein Knoten komplett ersetzt werden soll. Dies vermeidet, dass der Knoten gemerged wird.
        *        Im Master steht danach der originale Update-Knoten.
        *
        * @param elementName der Name des Element-Knotens
        *
        * @return true, falls das Element zu dieser Knotenart gehoert
        */
        bool shouldReplaceNode(const char* elementName);

        bool checkZFISize( tinyxml2::XMLElement* updateChildElement, 
             tinyxml2::XMLElement* masterChild, const char* zfiName);

        /**
        * @brief Hilfsmethode zum Checken, ob die Registrierung geaendert werden soll.
        *
        * Hierbei kann angegeben werden, dass Aenderungen der Vorregistrierung ueberschrieben werden sollen oder nicht (updateFlag = FORCE_INACCURATE).
        * Generell ueberschreibt eine Registrierung eine Vorregistrierung immer.
        *
        * @param updateFlags    generelle Flags fuer den Update-Modus
        * @param masterFlag     register flag des Masterprojekts
        * @param clientFlag     register flag des Updateprojekts
        *
        * @return true, falls die Registrierung uebernommen werden soll, ansonsten false
        */
        ZFPRJ_API bool updateOfRegistrationNeeded(int updateFlags, int masterFlag, 
            int clientFlag);

        /**
        * @brief Compares the given nodes lexicaly.
        *
        * @param node1 the first node
        * @param node2 the second node
        *
        * @return true, if the two nodes are equal, false otherwise
        */
        ZFPRJ_API bool compareNodes( tinyxml2::XMLNode* node1,  tinyxml2::XMLNode* node2);

        /**
        * @brief Compares the given nodes matrix wise.
        *
        * @param node1 the first node containing a matrix
        * @param node2 the second node containing a matrix
        *
        * @return true, if the matrices of the two nodes are equal, false otherwise
        */
        ZFPRJ_API bool compareMatrixNodes(
            tinyxml2::XMLNode * masterRegisterNode, 
            tinyxml2::XMLNode * updateRegisterNode,
            double epsilon);

        /********** Testmethoden ***********************************/
        ZFPRJ_API void print( tinyxml2::XMLNode* node);
        ZFPRJ_API void logMessage(const QString& message);
    };

} // end namespace

