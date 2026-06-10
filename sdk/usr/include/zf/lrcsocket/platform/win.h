#pragma once
#include "../lrcsocketapi.h"

#include <string>

// additional stuff needed by windows

LRCSOCKET_API bool WinWSAStart();
LRCSOCKET_API void WinWSAStop();



class LRCSOCKET_API Winsocket
{
public:
    Winsocket()
    {
        WinWSAStart();
    }

    ~Winsocket()
    {
        WinWSAStop();
    }
};


namespace zfsocket
{

    LRCSOCKET_API
        std::string addr_2_ipv4(const std::string& addr);

}//end namespace

