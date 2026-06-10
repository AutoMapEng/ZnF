#pragma once
#include "zfxyzapi.h"
/**
 * @brief Interface to store class for zfxyz files
 *
 * The basic file format ist follows:
 @verbatim
 |magic number|options|header-lenght|header|lut-length|lut|line 1|...|line n

 magic number : ZFXYZ002 => 8 bytes
 options      : 8 bytes for options
 header-length: 8 bytes for the lenght of following header data
 header       : Header information encoded as string
 lut-lenght   : 8 bytes for lenght of lut data
 lut          : Look up data for debugging purposes
 line         : The lines of the scan
 @endverbatim

 lut-length and lut only appear within the file if the option OPT_HAS_LUT is
 set within the options field.

 This file format can be read by new newer laser control versions (>= 8.6).
 */


#include <string>
#include <vector>
#include <stdint.h>

namespace zfxyz
{
    class ZFLineStreamStoringImpl;
    class ZFSHeaderStr;
    struct CorrectVect;
    struct XYZPixelIn;




    /**
        Store datastream in a file with extension .zfxyz
        LaserControl support reading this filetyp
    */
    class ZFXYZ_API ZFLineStreamStoring
    {
    public:
        ZFLineStreamStoring();
        virtual ~ZFLineStreamStoring();

        /**
         * @brief Start recording using filname as file path.
         *
         * The full file name will be generated from the following pattern:
         *
         * filename.\<partnumber\>.zfxyz
         *
         * Where parnumber will start at 000 and continue with 001 and so on as parts
         * grows lager than the maximum part size.
         *
         * @note Recoding will not start until a valid zfs-Header is provided as it
         * is integral part of every scan file.
         *
         * @warning Does overwrite already existing files!
         *
         * @see writeLine()
         * @see setHeader()
         *
         * @param filename Path where to write scans
         * @return true on success
         */
        bool create(const std::wstring& filename);

        /**
         * @brief Closes the current part and stops recording
         */
        void close();

        /**
         * @brief Set the header information for the current scan
         *
         * @note without calling this function (with header.items.size() > 0) no
         *       recording will take place.
         *
         * @param header Header data provided by scanner
         * @return true if header was set successfully
         */
        bool setHeader(const ZFSHeaderStr &header);
        bool hasHeader() const;

        /**
         * @brief Set the correction vector look up table (lut) to be stored within
         *        zfxyz file
         *
         * This method will make a copy of the vector for internal use.
         *
         * @note call this function only once before start
         *
         * @param cv [in] Reference to std::vector of correction vector elements
         * @return true on success
         */
        bool setLut(const std::vector<CorrectVect> &cv);
        bool hasLut() const;

        /**
         * @brief Write a line to the current part
         *
         * Line gets written to file only if header is set. If the header isn't already
         * written to the current part but set, the header is written by this function.
         *
         * A new new file (part) is started if the current file size + the bytes to
         * write would exceed the maximum part size.
         *
         * @see setHeader()
         *
         * @param lineheader [in] Header of current scan line
         * @param sizeLineheader  Size of header in bytes [positive value]
         * @param pixelData [in]  Array with pixel data in xyz format
         * @param nPixel          Number of elements in pixelData [positive value]
         * @return true on success
         */
        bool writeLine(
            const char* lineheader,
            int32_t sizeLineheader,
            const XYZPixelIn* pixelData,
            int nPixel); 
        
        /// if file is open for writing
        bool isOpen() const;

        std::wstring getBaseFileName() const;

        /// size of the current part [bytes]
        uint64_t partSize() const;
        
        /// size of all parts [bytes]
        uint64_t size() const;

        /// current part beginning with 0
        int32_t currentPart() const;

        /// lines written to file
        uint32_t linesWritten() const;

    protected:
        ZFLineStreamStoringImpl* impl;
    };
}
