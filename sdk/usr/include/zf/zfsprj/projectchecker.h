#ifndef LRV_PROJECTCHECKER_H
#define LRV_PROJECTCHECKER_H

#include "Project.h"
#include "ZfsPrjBasic.h"
#include "config.h"

#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <QObject>
#include <QProgressBar>
#include <QString>
#include <QWidget>
SWITCH_WARNINGS_ON

//=============================================================================

namespace zfprj {
#ifdef ZFPRJ_SLAM_SUPPORT
    class SlamResult;
#endif

    /**
     * @brief Class to store the results of a project check.
     */
    class ZFSPRJ_API ProjectCheckResult {

    public:
        /**
        * @brief Status of the overall result.
        */
        enum CheckStatus {
            CHECK_UNKNOWN = 0,
            CHECK_OK = 1,
            CHECK_WARNING = 2,
            CHECK_ERROR = 3
        } checkStatus;

        /**
         * @brief The check result messages (errors and warnings)
         */
        QStringList messages;

        /**
         * @brief the error count
         */
        int errors;

        /**
         * @brief the warning count
         */
        int warnings;

        ProjectCheckResult() : checkStatus(CHECK_OK), errors(0), warnings(0) {}

        /**
         * @brief Clears all check results.
         */
        void clear();

        /**
         * @brief Add the message to the result and set the status.
         *
         * Status is only changed if the severety is higher than the current one.
         * The counters are increased according the given status.
         *
         * @param message    the message to add
         * @param status     the last status
         */
        void addMessage(const QString message, CheckStatus status);
    };

    //=============================================================================

    /**
     * @brief Widget to display progress of running checks.
     */
    class ZFSPRJ_API CheckProgressWidget : public QProgressBar {
        Q_OBJECT

            ProjectCheckResult::CheckStatus status;

    public:
        /**
        * @brief The display message
        */
        QString message;

        /**
         * @brief the current progress value
         */
        double progress;

        CheckProgressWidget(QWidget *parent = 0, QString _message = "")
            : QProgressBar(parent), status(ProjectCheckResult::CheckStatus::CHECK_OK),
            message(_message) {
            changeProgress(0, message, status);
        }

        ~CheckProgressWidget() {};

        public slots:
        /**
        * @brief Slot to set the current status.
        *
        * The color of the progress bar is updated according the status.
        *
        * @param status the new status to be displayed
        */
        void setStatus(zfprj::ProjectCheckResult::CheckStatus status);

        void changeProgress(int progress, const QString &message,
            ProjectCheckResult::CheckStatus status =
            ProjectCheckResult::CHECK_UNKNOWN);
    };

    //=============================================================================

    /**
     * @brief This class provides consistency checks for the current project.
     *
     * The checks include file checking (scan files, ZFI container, additional files
     * and check for download fragments.
     *
     */
    class ZFSPRJ_API ProjectChecker : public QObject {
        Q_OBJECT

    public:
        int progress;
        bool canceled = false;

        ProjectChecker() {}

    public:
        bool checkForCancel(zfprj::ProjectCheckResult & result);

        /**
        * @brief Checks if the scan files for the current project are available.
        *
        * Improvements: we could check if the scan file has valid content...
        *
        * @param result the ProjectCheckResult reference where the results are added
        * to.
        */
        void checkScans(ProjectCheckResult &result);

        /**
         * @brief Checks if the additional files for the scans in the current project
         * are available.
         *
         * @param result the ProjectCheckResult reference where the results are added
         * to.
         */
        void checkAdditionalFiles(ProjectCheckResult &result);

        /**
         * @brief Checks if the link files in the current project are available.
         *
         * @param result the ProjectCheckResult reference where the results are added
         * to.
         */
        void checkLinkFiles(ProjectCheckResult &result);

        /**
         * @brief Checks if there are temporary download files in the project folder.
         *
         * @param result the ProjectCheckResult reference where the results are added
         * to.
         */
        void checkDownloadFragments(ProjectCheckResult &result);

        void checkBackgroundFiles(ProjectCheckResult &result);

        /**
         * @brief Add the given message to the result and emit the
         * notifyMessage-signal.
         *
         * @param result     the ProjectCheckResult reference where the results are
         * added to.
         * @param message    the new message
         * @param status     the CheckStatus of the new message (CHECK_ERROR,
         * CHECK_WARNING etc.)
         */
        void addMessageToResult(ProjectCheckResult &result, const QString message,
            ProjectCheckResult::CheckStatus status);

    signals:
        /**
        * @brief Signals a new check result message.
        *
        * @param message    the message to be send
        * @param status     the current status
        */
        void notifyMessage(const QString &message,
            ProjectCheckResult::CheckStatus status);

        /**
         * @brief Signals a new check progress.
         *
         * @param progress   the current progress value (in percent)
         * @param message    the current progress message
         * @param status     the current status.
         */
        void notifyProgress(int progress, const QString &message,
            ProjectCheckResult::CheckStatus status);
    };

    //=============================================================================

    /**
     * @brief This class provides checks if the current project conflicts with the
     * given project.
     *
     * The checks include file checking (scan files and additional files) and
     * viewpoint comparison. It also handles the copy process if the copy flag is
     * set in the according methods.
     */
    class ZFSPRJ_API ProjectConflictChecker : public QObject {
        Q_OBJECT

        QString importProjectName; ///< the name of the project to be imported
        ZFProject *importProject;  ///< the loaded project
        ProjectCheckResult result; ///< the current check result

        QStringList
            importedScansInSubfolder; ///< list contains the scan names of scans
                                      ///< imported from a project in a subfolder
        QString importSubfolder;

    public:
        ProjectConflictChecker(const QString &importProjectName);
        void init();
        ~ProjectConflictChecker();

    protected:
#ifdef ZFPRJ_SLAM_SUPPORT
        bool copySlamResult(const zfprj::SlamResult* importResult, const zf::FileName& copyPath, QString& resultText);
#endif

    public:
        const QStringList &getImportedScansInSubfolder() {
            return importedScansInSubfolder;
        }

        const QString &getSubFolder() { return importSubfolder; }

        ZFProject *getImportProject() { return importProject; }

        void resetResult();

        /**
         * @brief Checks the attributes of the current and the merge project.
         */
        void checkProjectAttributes();

        /**
         * @brief Checks if the coordiante systems of the projects can be merged.
         */
        void checkCoordinateSystems();

        /**
         * @brief Checks if the groups of the projects can be merged.
         */
        void checkGroups();

        /**
         * @brief Checks if the scan files for the projects can be merged.
         *
         * @param copy   if this is set to true, the scan files are copied
         * @param force  if this flag is true, files are overridden
         */
        void checkScans(bool copy = false, bool force = false);

#ifdef ZFPRJ_SLAM_SUPPORT
        /**
        * @brief Checks if the slam files for the projects can be merged.
        *
        * @param copy   if this is set to true, the scan files are copied
        * @param force  if this flag is true, files are overridden
        */
        void checkSlam(bool copy = false );
#endif
        /**
         * @brief Checks if the additional files can be merged.
         *
         * @param copy   if this is set to true, the files are copied
         * @param force  if this flag is true, files are overridden
         */
        void checkAdditionalFiles(bool copy = false, bool force = false);

        /**
         * @brief Checks if the targets can be merged.
         *
         * @param force  if this flag is true, target conflicts are ignored
         */
        void checkTargets(bool copy = false, bool force = false);

        /**
         * @brief Checks if the files for backgrounds can be merged
         *
         * @param copy   if this is set to true, the scan files are copied
         * @param force  if this flag is true, files are overridden
         */
        void checkBackgroundFiles(bool copy = false, bool force = false);

        void checkBackgroundFile(QString &backgroundImageName, bool copy);

        /**
         * @brief Checks if the temp files for the projects can be merged.
         *
         * @param copy   if this is set to true, the scan files are copied
         * @param force  if this flag is true, files are overridden
         */
        void checkTempFiles(bool copy = false, bool force = false);

        /**
         * @brief Checks if the link files for the projects can be merged.
         *
         * @param copy   if this is set to true, the link files are copied
         * @param force  if this flag is true, files are overridden
         */
        void checkLinkFiles(bool copy = false, bool force = false);

        /**
         * @brief Add the given message to the result and emit the
         * notifyMessage-signal.
         *
         * @param message    the new message
         * @param status     the CheckStatus of the new message (CHECK_ERROR,
         * CHECK_WARNING etc.)
         */
        void addMessageToResult(const QString message,
            ProjectCheckResult::CheckStatus status);

        /**
         * @brief Get the result of the current check
         *
         * @return the current result
         */
        ProjectCheckResult getResult() { return result; }

    signals:
        /**
        * @brief Signals a new check result message.
        *
        * @param message    the message to be send
        * @param status     the current status
        */
        void notifyMessage(const QString &message,
            ProjectCheckResult::CheckStatus status);

        /**
         * @brief Signals a new check progress.
         *
         * @param progress   the current progress value (in percent)
         * @param message    the current progress message
         * @param status     the current status.
         */
        void notifyProgress(int progress, const QString &message,
            ProjectCheckResult::CheckStatus status);
    };    
} // namespace zfprj
#endif
