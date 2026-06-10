#pragma once

#include "lrcsocketapi.h"

#include <memory>
#include <string>
#include <utility>
#include <functional>
#include <vector>
#include <stdint.h>


namespace zf
{

    class UdpStreamImpl;

    /**
    \brief  implementation for a raw udp stream
            split a udp stream depending on addr of sender
            a receiver thread is started
    */
    class LRCSOCKET_API UdpStream
    {
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
        std::shared_ptr<UdpStreamImpl> impl{};
#pragma warning(pop)

    public:
        UdpStream();
        virtual ~UdpStream();

        bool openSocket(uint16_t port, const std::string& addr = "");
        void closeSocket();

        /// return socket
        int getSocket() const;

        /// sizeof(sockaddr_in)
        int sizeofAddr() const;

        /// receive bytes from socket
        // return bytes, addr of sender
        std::pair<int,std::string> recmsg(char* buf, int sz);

        // add a callback for receiving data depending on addr
        typedef std::function< void(const std::vector<char>&, int len) > rec_cb;
        void addCB(const std::string& addr, rec_cb);
        void delCB(const std::string& addr);
        void delAllCB();


        // lowlevel

        /// recvfrom. wait for data
        int rec(char* buf, int sz);

        // experimental
        std::string addrString() const;

    };
}

