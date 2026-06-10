#pragma once
#ifdef _WINDOWS
#include "../lrcsocketapi.h"

#include <string>
#include <utility>

namespace zf
{

	//LRCSOCKET_API void SendToCliendAndClose( const char* ip, int port, const char* str, int len=0 );

	LRCSOCKET_API const std::pair<bool,std::string> SendToCliendAndWaitForReturn(const std::string& ip, int port, const std::string& str, int to = 1000);

}
#endif // _WINDOWS
