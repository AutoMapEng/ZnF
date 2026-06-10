#pragma once
#include "zfsapi.h"

/**
 * @brief Class to serialize access to ZFS header from multiple threads
 *
 * Creating an instance of this class locks an global mutex which prevents
 * creating other instances of this class at the same time.
 * Deleting the object frees also the mutex.
 *
 * @note is a NOOP on any other platform than firmware!
 */

class ZFS_API ZFSHeaderMutex
{
public:
    ZFSHeaderMutex();
    ~ZFSHeaderMutex();
};
