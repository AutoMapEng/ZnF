#pragma once

#include <lrcsocket/lrcsocket.h>


#define USE_INTERFACE
#define USE_LINESTREAM
#define USE_INFOSTREAM


#ifndef PI
# define PI 3.1415926535897932
#endif

namespace zf
{
    namespace ctrl
    {
        //=========================================================================================


        /*
        TCP/IP Command interface
        */

#ifdef USE_INTERFACE
        class MyLRC : public LRCInterface
        {
        public:
            // !!!! must be thread save !!!
            virtual void OnMessagebox(int nr, const char* msg, msgtyp_t act, const char* info)
            {
            }

            // !!!! must be thread save !!!
            virtual void OnEvent(const char* ev)
            {
            }

            virtual void OnConnect();
        };
#endif // USE_INTERFACE

        //=========================================================================================
    } // end namespace
}// end namespace

