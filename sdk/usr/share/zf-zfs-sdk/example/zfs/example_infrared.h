#pragma once


#ifdef USE_OPENCV // defined via cmake
#define INFRAREAD_EXAMPLE
#endif


#ifdef INFRAREAD_EXAMPLE

#include <string>

//========================================================================================================================
void Sample_LineByLineAccess_Infrared(const std::wstring& win);
//========================================================================================================================

#endif // INFRAREAD_EXAMPLE

