#pragma once
#ifdef _WINDOWS // im Moment kein Linux
#include "../lrcsocketapi.h"

#include <thread>
#include <mutex>
#include <list>
#include <memory>
#include <string>

#include <WinSock2.h>

class ZFServer;

#pragma warning(disable: 4251 )
// 4251 disable DLL Schnittstelle


/**
\brief a socket connection in its own thread
*/
class LRCSOCKET_API ZFServerCon
{
    friend class ZFServer;

public:
    ZFServerCon( ZFServer*, SOCKET& client, sockaddr_in&);
    virtual ~ZFServerCon();

    /// get address string of connection
    const char* addr() const { return adr; }

    virtual void OnRead( const char*, int /*len*/ ) {}
    virtual void OnClose( SOCKET&, sockaddr_in& ) {}

    ///send buffer
    bool Send( const char* buf, int len );
    bool Send( const char* buf );
    bool Send(const std::string& buf);

	bool wantsNotifications() const;

	/// return tcp server who hold the connection
//	ZFServer* server() { return parent; }
//    const ZFServer* server() const { return parent; }

protected:
    void ClientThread();

    SOCKET client{};
    ZFServer* parent{};
    sockaddr_in from{};
    char adr[20]{};
    //make notification for client switchable (not yet implemented)
    bool m_bNotify{};//true if this socket is included in notification broadcasts
};



/**
\brief a server thread listen for incomming connections requests
*/
class LRCSOCKET_API ZFServer
{
    friend class ZFServerCon;

public:
    ZFServer( int port, int maxcon=10 );
    virtual ~ZFServer();

    virtual ZFServerCon* OnNewConnection( SOCKET&, sockaddr_in& );
	virtual void OnCloseConnection(ZFServerCon*) {}

    /// UTF8
	void notifyClients(const char* pMsg);
    void notifyClients(const std::string& msg);
    /// UTF16
    void notifyClients(const std::wstring& msg); 

    bool isRunning() const { return running; }

    bool waitRunning(int to) const;

    /// maximum supported connections
    int maxConnections() const { return maxconnections; }
    
    /// current open connections
    int connections() const;

protected:
    void MTServerThread();
    SOCKET server{};
    int    maxconnections{};
    int    port{};
    bool   running{}; ///< running
    int   error{};  ///< start error
    mutable std::mutex cs{};
    std::thread th{};
    std::list< std::shared_ptr<ZFServerCon> > conlist;

protected:
    void destroy();
    void remove(ZFServerCon*);
};

#endif // _WINDOWS
