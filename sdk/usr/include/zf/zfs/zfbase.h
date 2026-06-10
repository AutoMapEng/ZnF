#pragma once

#include "zfsapi.h"

#include <string.h>
#include <list>
#include <memory>

//#include <mutex> // this is not allowed because otherwise no common language support is possible


//
// base class for all ZF classes
//
//    implements stuff for class handling
//

namespace zfs
{
    class ZFLock;
}

namespace std
{
    class mutex;
}


/**
\brief base calss for all scans and objects
*/
class ZFS_API ZFBase
{
    static const char* cname;       ///< the class name
    std::list<zfs::ZFLock*> locks;  ///< a list of locks
    bool bKill{};                     ///<
    std::shared_ptr<std::mutex> mutLocks;

    void swap(ZFBase& first, ZFBase& second);

protected:
    ZFBase* pParentIfClone{};	// parent scan if it is a clone

public:
    ZFBase();
    ZFBase(const ZFBase& other);    ///< Custom copy constructor does not copy the locks, so an copied object will never be locked.
    virtual ~ZFBase();

    ZFBase& operator=(ZFBase other);

    static const char* getClassName() { return cname; }
    virtual const char* className() const { return cname; }
    virtual bool isKindOf(const char* n) const { return strcmp(n, ZFBase::getClassName()) == 0; }

    void AddLock(zfs::ZFLock*);
    void ReleaseLock(zfs::ZFLock*);
    bool IsLocked() const;
    void ResetLocks();
    int GetNumberOfLocks() const;

    void Kill();
    bool IsKilled() const;
    bool WaitKilled(int to = 100);
};

//------------------------------------------------------------------------------------------------

// define in you class definition
#define DEFINE_ZFBASE(basename) \
    private:\
    static const char* cname; \
    public: \
    static const char* getClassName() { return cname; } \
    const char* className() const override { return cname; } \
    bool isKindOf( const char* n) const override { if( strcmp(n,getClassName())==0 ) return true; return basename::isKindOf(n); } \


// define in you implemation cpp file
#define IMPLEMENT_ZFBASE(classname) \
    const char* classname::cname = #classname
