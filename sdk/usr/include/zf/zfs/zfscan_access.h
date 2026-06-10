#pragma once

#include "zfsapi.h"

class ZFScanBasic;
class ZFDataAccess;

namespace zfs
{
    /**
        \brief Allow multithreaded access to scan pixels
    */
    class ZFS_API ZFPixelAccess
    {
        ZFScanBasic* z{};      ///< pointer to scan
        ZFDataAccess* data{};   ///< the scan data itself

    public:
        int currentPixel{};
        int currentLine{};
        int currentPixelMap{};    //!< Current pixel after mapping.
        int currentLineMap{};     //!< Current line after mapping.
        char* ptr{};				//!< pointer to current pixeldata
        int id{};					//!< for debugging

    public:
        ZFPixelAccess(ZFScanBasic* _z) : z(_z) {}
        virtual ~ZFPixelAccess() {}

        void Init(ZFDataAccess* da) { data = da; }
        void ResetCurrent()
        {
            currentPixel = currentLine = 0;
            currentPixelMap = currentLineMap = 0;
        }


        int CurrentPixel() const { return currentPixel; }
        int CurrentLine() const { return currentLine; }
        int CurrentPixelMap() const { return currentPixelMap; }
        int CurrentLineMap() const { return currentLineMap; }
        char* PPtr() { return ptr; }
        const char* PPtr() const { return ptr; }

        bool Supported()
        {
            return data ? true : false;
        }

    };
} // end namespace zfs

