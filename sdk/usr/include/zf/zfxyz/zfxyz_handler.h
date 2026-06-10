#pragma once
#include "zfxyzapi.h"

#include <string>



namespace zfxyz
{
    class ProtocolHandlerImpl;
    class ProtocolHandler;
    struct XYZPixelIn;
    class ZFLineStreamStoring;


    /**
    \brief can store the datastream to disk and call other functions
    */
    class ZFXYZ_API XYZStreamHandler
    {
    public:
        XYZStreamHandler();
        virtual ~XYZStreamHandler();

        /// define a callback function which ist called for every line. You can print out the current line for example
        typedef void(*cb_newline_f)(void* context, char* lineheader, int sizeLineheader, XYZPixelIn* pixelData, int nPixel);
        void setCallbackNewLine(void* context, cb_newline_f);

        /// set the base filename
        void setBaseFilename(const std::wstring& fn);

        std::wstring getBaseFilename() const;

        void Reset();

        /// add the binary raw data receicing from the socket
        void Add(const char* buffer, int bytes);

        /// enable/disable storing
        void setStore(bool enable);

        /// get access to the storer class to call for additional information while storing like bytes written
        const ZFLineStreamStoring& storingHandler();

        /// get an reference to the Protocol handler
        ProtocolHandler& getProtoHandler() const;

    protected:
        ProtocolHandlerImpl* impl;
    };
}
