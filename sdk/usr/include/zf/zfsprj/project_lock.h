#pragma once

#include "ZfsPrjBasic.h"

class ZFProject;

#include "../Common/def.h"
SWITCH_WARNINGS_OFF
#include <QReadWriteLock>
SWITCH_WARNINGS_ON


/**
*   Klasse zum setzten eines Lese/Schreib mutex fuer das Projekt.
*/
class ZFSPRJ_API ProjectReadWriteSyncLock
{
private:
    bool lock;
    bool mReadWrite;
    QReadWriteLock& sObj;
public:
    /**
    *   \param o => projekt das gespert werden soll.
    *   \param readWrite => true = Lese und Schreib zugriff werden geblockt. false = Es wird nur schreiben geblock. Lesende zugriffe sind beliebig viel moeglich.
    *	\param tryLock_ => true = Es wird versucht zu locken falls das nicht geht geht die Methode weiter. Danach muss mit isLocked() abgefragt werden ob das Locken erfolgreich war.
    */
    ProjectReadWriteSyncLock(ZFProject& o, bool readWrite, bool& tryLock_);
    ProjectReadWriteSyncLock(ZFProject& o, bool readWrite);
    ProjectReadWriteSyncLock(QReadWriteLock& o, bool readWrite);

    ~ProjectReadWriteSyncLock()
    {
        release();
    }

    /**
    *   Mutex loesen
    */
    void release()
    {
        if (lock)
        {
            lock = false;
            sObj.unlock();
        }
    }

    /**
    *   Mutex setzten
    */
    void acquire()
    {
        lock = true;
        if (mReadWrite)
            sObj.lockForWrite();
        else
            sObj.lockForRead();
    }

    bool tryLock() {
        lock = true;
        bool ok;
        if (mReadWrite)
            ok = sObj.tryLockForWrite(5000);
        else
            ok = sObj.tryLockForRead(5000);
        return ok;
    }

    bool isLocked() { return lock; }

private:
    ProjectReadWriteSyncLock(const ProjectReadWriteSyncLock& o) : sObj(o.sObj) { }
    ProjectReadWriteSyncLock& operator=(const ProjectReadWriteSyncLock&) { return *this; }
};
