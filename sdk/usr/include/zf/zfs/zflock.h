#ifndef _ZFS_LOCK_H
#define _ZFS_LOCK_H

#include "zfsapi.h"

#include <list>

class ZFBase;

namespace zfs
{

/**
    Locking
*/
class ZFS_API ZFLock
{
public:
    
    ZFLock();
	virtual ~ZFLock();
};



//######################################################################

class ZFS_API ZFLocking
{
	ZFLock* pLock;
	ZFBase* pBase;

public:
    ZFLocking( ZFBase* b  );
	virtual ~ZFLocking();
};


} // end namespace zfs

#endif // _ZFS_LOCK_H
