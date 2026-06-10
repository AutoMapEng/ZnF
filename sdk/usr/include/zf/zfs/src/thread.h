#pragma once

/*
    Copyright (C) 2008 by Zoller & Froehlich Gmbh Germany
*/


#include "../config_zfs.h"

#include <mutex>


//=====================================================

class ZFThreadMutex
{
    std::mutex mut;

public:
    ZFThreadMutex() = default;

    void Lock()
    {
        mut.lock();
    }

    void Unlock()
    {
        mut.unlock();
    }
};


class ZFThreadMutexLock
{
protected:
    ZFThreadMutex& mx;

public:
    ZFThreadMutexLock( ZFThreadMutex& m )
        : mx( m )
    {
        mx.Lock();
    }
    virtual ~ZFThreadMutexLock()
    {
        Unlock();
    }

    void Unlock()
    {
        mx.Unlock();
    }

private:
    ZFThreadMutexLock& operator=( const ZFThreadMutexLock&) {return (*this);}
};



