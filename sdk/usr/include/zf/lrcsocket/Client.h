#pragma once

#include <string>

#include "lrcsocketapi.h"
#include "platform/zfsocket.h"

/// Maximum amount of bytes the ZFTCPClient an receive at once
#define ZFTCPCLIENT_RD_MAX	(65536*4)

class ZFTCPClient_Impl;

/**
 * @brief TCP/IP client socket
 *
 * Can be used to esablish an TCP communication channel to a server and
 * to send/receive data.
 *
 * @note Receiving is done asynchronously in a separate thread!
 */
class LRCSOCKET_API ZFTCPClient
{
    /* cannot be copied! */

    ZFTCPClient(ZFTCPClient const &o) { }
    ZFTCPClient& operator=(ZFTCPClient const &) { return *this; }

public:
    /** @brief Creates a new unconnected client socket */
    ZFTCPClient();

#if __cplusplus >= 201103L
    /**
     * @brief Move construct client socket 
     * @param o ZFTCPClient to take ownership of
     */
    ZFTCPClient(ZFTCPClient &&o);

    /** 
     * @brief Move assign client socket 
     * Effectively o and this are exchanged
     * @param o ZFTCPClient to take ownership of
     */
    ZFTCPClient &operator=(ZFTCPClient &&o);
#endif // __cplusplus >= 201103L

    /** @brief Closes the client socket and frees all ressources */
    virtual ~ZFTCPClient();

    /**
     * @brief Establish connection to server:port
     *
     * An IPv4 or IPv6 connection may be established
     *
     * @param servername Hostname or IP address to connect to
     * @param port Portnumber to connect to
     * @return true on success
     */
    virtual bool Connect(const char* servername, int port);

    /**
     * @brief Closes the connection
     * @param waitThread ignored parameter
     * @return true on success
     */
    virtual bool CloseCon(bool waitThread = true);

    /**
     * @brief Called by by background thread while exiting
     */
    virtual void OnClosed(int) {}

    /**
     * @brief Called by background thread if new data was received
     *
     * @param data Pointer to received data
     * @param len  number of bytes available
     */
    virtual void OnReadRaw(const char* data, int len) {}
    virtual void OnReadXML(const std::string&) {}

    /**
     * @brief Enable TCP keepalive option
     *
     * This option is enabled by default. To take effect, this function must
     * be called before Connect() is called.
     *
     * @param keepalive true to enable option, false to disable option
     */
    void Keepalive(bool keepalive);

    /**
     * @brief Inform weather keepalive option is set or not
     * @return true if keepalive option is set
     */
    bool Keepalive() const;

    /**
     * @brief Send bytes to server
     *
     * @param buf Pointer to bytes to send
     * @param len Number of bytes to send
     * @return true on success
     */
    bool Send(const char* buf, int len);

    /**
     * @brief Send C string to server
     *
     * @param buf pointer to C string
     * @return true on success
     */
    bool Send(const char* buf);

    /**
     * @brief Return the IP address or server name Connect() was called with
     * @return IP address or server name
     */
    const char* Server() const;

    /**
     * @brief Return the port number Connect() was called with
     * @return port number
     */
    unsigned short Port() const;

    /**
     * @brief Return the OS specific socket handle
     * @return int on unix, SOCKET on Windows
     */
    lrcsocket::zfsocket NativeHandle() const;


private:
    ZFTCPClient_Impl *m_impl{};
};



//############################################################################################


/**
 * @brief Determines the local IP the socket sock is bound to
 * @param sock Socket
 * @return IPv4 or IPv6 address as string or empty string if failed
 */
LRCSOCKET_API std::string LocalAddress(lrcsocket::zfsocket sock);

/**
 * @brief Determines the remote IP the socket is bound to
 * @param sock Socket
 * @return IPv4 or IPv6 address as string or empty string if failed
 */
LRCSOCKET_API std::string RemoteAddress(lrcsocket::zfsocket sock);

/** @overload std::string LocalAddress(lrcsocket::zfsocket sock) */
LRCSOCKET_API std::string LocalAddress(const ZFTCPClient &client);

/** @overload std::string RemoteAddress(lrcsocket::zfsocket sock) */
LRCSOCKET_API std::string RemoteAddress(const ZFTCPClient &client);


