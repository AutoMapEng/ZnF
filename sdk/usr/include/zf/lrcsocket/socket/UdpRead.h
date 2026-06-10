#pragma once
#include "../lrcsocketapi.h"
#include <zfcore/compiler.h>

#include <memory>

class CUDPReadImpl;

/**
\brief receive datagrams from a socket via UDP/IP
*/
class LRCSOCKET_API CUDPRead
{
public:
	CUDPRead();
	virtual ~CUDPRead();

    virtual bool Create( int port );

    typedef void(*cb_read_f)(void* context, const char* buf, int len, const char* addr );
    void setCB(void* con, cb_read_f f);

    virtual void OnRead( char* buf, int len ) {}

protected:
    std::unique_ptr<CUDPReadImpl> impl{};
};

