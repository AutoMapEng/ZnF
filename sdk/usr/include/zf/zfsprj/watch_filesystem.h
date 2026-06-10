#pragma once

#include "ZfsPrjBasic.h"
#include <zfcore/compiler.h>
#include <functional>

#include <Common/def.h>
SWITCH_WARNINGS_OFF
#include <QObject>
#include <QString>
#include <QStringList>
SWITCH_WARNINGS_ON

class ZFProject;
class QFileSystemWatcher;

namespace zfprj
{
    /**
    @brief File system watcher
    */
    class ZFSPRJ_API ZFProjectWatcher : public QObject
    {
        Q_OBJECT

    public:
        ZFProjectWatcher();
        virtual ~ZFProjectWatcher();

        typedef std::function< void(int, const std::wstring& file) > cb_t;
        void cb(cb_t f) { m_cb = f; }

        /// add directory for watching
        void watch(const std::wstring& path);

        /// remove all
        void destroy();

    protected:
        QFileSystemWatcher* watcher{};
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
        QStringList last;
        QStringList extensionsToWatch;
#pragma warning(pop)
        cb_t m_cb{};

    protected slots:
        void dirChanged(const QString& path);
        void fChanged(const QString& fn);
    };
}