#pragma once

#ifndef PI
# define PI 3.1415926535897932
#endif

#ifndef TORAD
# define TORAD(grad)  ( (double)(grad)*(PI/180.0))
#endif

#ifndef TOGRAD
# define TOGRAD(rad)  ( (double)(rad )*(180.0/PI) )
#endif

// macros for switching warnings off and on in precompiled headers
#ifdef WIN32

#define SWITCH_WARNINGS_OFF __pragma(warning(push)) \
__pragma(warning(disable: 4018)) /* numerical warning: conflict signed vs. unsigned comparison */\
__pragma(warning(disable: 4267)) /* numerical warning: init/conversion size_t to int */\
__pragma(warning(disable: 4244)) /* numerical warning: init/conversion double in int */\
__pragma(warning(disable: 4305)) /* numerical warning: init. shortening double to float */\
__pragma(warning(disable: 4309)) /* numerical warning: static_cast shortening of a constant value */\
__pragma(warning(disable: 4800)) /* numerical warning: variable is set to boolean value */\
__pragma(warning(disable: 4003)) /*level1; macro; number of parameters for macro wrong (macros shouldn't be used)*/\
__pragma(warning(disable: 4005)) /*level1; macro; macro redefinition (macros shouldn't be used)*/\
__pragma(warning(disable: 4099)) /* */\
__pragma(warning(disable: 4100)) /*unreferenced formal parameter*/\
__pragma(warning(disable: 4121)) /* */\
__pragma(warning(disable: 4127)) /*conditional expression is constant */\
__pragma(warning(disable: 4172)) /* */\
__pragma(warning(disable: 4189)) /* */\
__pragma(warning(disable: 4190)) /* */\
__pragma(warning(disable: 4201)) /* */\
__pragma(warning(disable: 4231)) /* */\
__pragma(warning(disable: 4239)) /* */\
__pragma(warning(disable: 4245)) /* */\
__pragma(warning(disable: 4251)) /*class "std::basic_string<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t>>" erfordert eine DLL-Schnittstelle, die von Clients von ...*/\
__pragma(warning(disable: 4275)) /* */\
__pragma(warning(disable: 4302)) /* */\
__pragma(warning(disable: 4311)) /* */\
__pragma(warning(disable: 4312)) /* */\
__pragma(warning(disable: 4324)) /* */\
__pragma(warning(disable: 4334)) /* */\
__pragma(warning(disable: 4355)) /* */\
__pragma(warning(disable: 4389)) /* */\
__pragma(warning(disable: 4456)) /* */\
__pragma(warning(disable: 4458)) /* */\
__pragma(warning(disable: 4459)) /* */\
__pragma(warning(disable: 4481)) /* */\
__pragma(warning(disable: 4503)) /* */\
__pragma(warning(disable: 4512)) /*assignment operator could not be generated */\
__pragma(warning(disable: 4535)) /* */\
__pragma(warning(disable: 4702)) /* */\
__pragma(warning(disable: 4706)) /* */\
__pragma(warning(disable: 4714)) /* */\
__pragma(warning(disable: 4913)) /* */\
__pragma(warning(disable: 4996)) /* */\
__pragma(warning(disable: 6011)) /* */\

#define SWITCH_WARNINGS_ON __pragma(warning(pop))
//_Pragma warning("pop")

#else //===== Linux ========

#define SWITCH_WARNINGS_OFF 
#define SWITCH_WARNINGS_ON 

#endif
