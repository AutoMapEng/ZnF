 #pragma once

#include "zfxyzapi.h"
#include "zfxyz_header.hpp"
#include "zfxyz_def.h"

#include <mutex>
#include <stdint.h>
#include <vector>

namespace zfxyz
{


    class pixel_enc;


    /**
     * @brief The ProtocolHandler class parses the streaming protocol
     *
     * Notifies the application via OnNewLine() and OnNewHeader() about newly
     * received lines or header.
     * Derive from this class and implement OnNewLine() and OnNewHeader() to
     * implement your application.
     */
    class ZFXYZ_API ProtocolHandler
    {
    private:
        std::vector<unsigned char> m_in; ///< buffer holding raw input data

        std::vector<XYZPixelIn> m_pXYZ;		///< memory for convertet xyz pixel line for transfering to application
        std::vector<unsigned char> m_lh;    ///< memory for line-Header

        unsigned int        m_lastLineNumber;	///< used for detecting lost lines

        /// Correction-Vector for each encoder position
        std::vector<CorrectVect> m_corrections;

        // ZFS Header
        bool         m_haszfsheader; ///< Valid ZFS header available
        ZFSHeaderStr m_zfsheader; ///< Last received ZFS header data

        std::mutex *m_mtx; ///< serializes invocations of Add()

        // options
        /// do include horizontal encoder value into xyz calculation. If true
        /// Scan lines might be shown deformed
        bool m_respect_horz_enc;

    public:

        // Statistic
        int m_nLines;           ///< number of lines received
        int m_nUnknownFrames;	///< number of unknown frames detected
        int m_nUnknownTyp;		///< number of unknown types detected

        ProtocolHandler();
        virtual ~ProtocolHandler();

        /**
         * @brief Enable or disable respection of horizontal encoder
         * @param v true to enable function
         */
        void setRespectHorzEnc(bool v)
        {
            m_respect_horz_enc = v;
        }

        /**
         * @brief Parse additional data
         *
         * Push newly received data to this function. If a ZFS header or a complete
         * line of XYZ-Data is received the corresponding callbacks will be called.
         *
         * @see OnNewLine()
         * @see OnNewHeader()
         *
         * @param data [in] Pointer to buffer
         * @param data_len number of bytes to parse (>= 0)
         */
        void Add(const void * data, int data_len);

        /**
         * @brief Reset internal parser state by discarding any received data
         */
        void Reset();

        /**
         * @brief Called if new Line has been parsed
         *
         * @param lineheader     adress to the lineheader
         * @param sizeLineheader the size of the lineheader in bytes
         * @param pixelData      the calculated xyz pixels
         * @param nPixel         the number of pixels in the structure
         */
        virtual void OnNewLine(char*     /*lineheader */,
            int       /*sizeLineheader*/,
            XYZPixelIn* /*pixelData */,
            int       /*nPixel*/)
        {}

        /**
         * @brief Called if new zfs header has been parsed
         *
         * @param h [in] Reference to zfs header structure
         */
        virtual void OnNewHeader(const ZFSHeaderStr& /*h*/)
        {}

        /**
         * @brief Called if a new LUT with correction values is received
         *
         * @param cv [in] Reference to vector to correction values
         */
        virtual void OnNewLUT(const std::vector<CorrectVect> & /*cv*/)
        {}

        bool hasZFSHeader() const;

    protected:

        /**
         * @brief Parse data and call callbacks (if set)
         * @param data     Protocoll data to parse
         * @param data_len lenght of data in bytes
         */
        int Parse(const unsigned char *data, int data_len);

        /**
         * @brief Decode XYZ data delivered via pixels and count
         *
         * @todo Take max Encoder values from ZFS-Header
         * @todo Cleanup!
         *
         * @param pixels Array of pixel structures received
         * @param count Number of pixel elements in Array
         * @return Number of decoded pixels
         */
        int CalcXYZ(zfxyz::pixel_enc *pixels, int count);

        /**
         * @brief Return a reference to the last received ZFS header
         * @return Reference to ZFS header
         */
        const ZFSHeaderStr& getHeader() const { return m_zfsheader; }

        void AllocXYZBuf(unsigned int pix_cnt);
    };
} // namespace

