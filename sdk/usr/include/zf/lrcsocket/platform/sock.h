#pragma once
#include <cstdint>

#include "zfsocket.h"

#ifdef _WINDOWS

#ifndef SOCKET
#include <winsock2.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

class C_Socket
{
public:
	int GetLastError() const;
//	char* GetLastErrorByNumber(int err);

protected:
	virtual ~C_Socket() {}

	WSADATA     wsadata;
	SOCKADDR_IN sockaddr;
	SOCKADDR_IN remoteaddr;
	SOCKET      sock;
	int         conn_type;
};

#endif // _WINDOWS