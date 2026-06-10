#pragma once
#include "lrcsocketapi.h"

#include <string>
#include <list>

#include "../Common/inttypes_.h"



namespace lrcsocket
{
    namespace tcpstreaming
    {
        class TCPSTreamingConnectionImpl;

        /**
        * !!! Not implemented yet
        * 
        * Downloadmanager for downloading files from scanner
        *   If download or connection breaks, the software will try to connect again anf procceed downloading the file
        *   The download will work while scanner already creates the file.
        *   The streaming may pause in the middle of a scan if there are no adtas available
        */
        class TCPSTreamingConnection
        {
        public:
            TCPSTreamingConnection();
            virtual ~TCPSTreamingConnection();

            /// Die Verbindungsparameter definieren
            void setup(const std::string& _ip, int _portCmd, int _portStream);

            void close();

            /// ask for downloading a file. Callback will inform you for fin
            void download(const std::wstring& srcScanner, const std::wstring& destLocal);

            typedef void(*cb_f)(const std::wstring&);
            void installCB(cb_f);

        protected:
            TCPSTreamingConnectionImpl* impl{};
        };
    } // end namespace
} // end namespace
