#pragma once

#include <stdio.h>
#include <string.h>
#include <string>

#include "zfsapi.h"
#include "config.h"
#include "flags.h"
#include "lineheader.h"

#include "../Common/inttypes_.h"

namespace zfs
{

    /**
        Access to items of lineheader
        holds just a pointer to orginal lineheader
    */
    class ZFS_API LineHeader
    {
    public:
        LineHeader()
        {
            m_lhsz = 0;
            m_lh = 0;
        }

        LineHeader( char* buf, int sz )
        {
            m_lh = 0;
            m_lhsz = 0;
            if( sz >= 4 )
            {
                m_lhsz = sz & ~3;
                m_lh = buf;
            }
        }

        ~LineHeader() { }

        enum { ID_LINEHEADER = 0x123455aa }; ///< the lineheader start id

        enum lh_t{
            LH_ID,              ///< id
            LH_ENC_V1,          ///< vertikal start 1
            LH_ENC_H1,          ///< horizontal start 1
            LH_ENC_V2,          ///< vertikal start 2 (not longer used)
            LH_ENC_H2,          ///< horizontal start 2 (not longer used)
            LH_REF_FLAGS,       ///< referenze rg and flags
            LH_REF_REF_RF,      ///< referenz diff and referenze rf (channel 1)
            LH_TIME,            ///< pc time added by software
            LH_LINENUMBER,      ///< linenumber
            LH_USER,            ///< userdata
            LH_LEVEL,           ///< leveling system
            LH_CNT1,            ///< counter 1
            LH_CNT2,            ///< counter 2
            LH_CNT3,            ///< counter 3 (future use)
            LH_GPS_LAT_low,     ///< gps latitude low word
            LH_GPS_LAT_high,
            LH_GPS_LONG_low,
            LH_GPS_LONG_high,
            LH_DEBUG_low,
            LH_DEBUG_high,
            LH_PIXEL_COUNTER,   ///< counts the pixel. Will overflow.

            _LH_,

            // not longer used. Type 415
            LH_REF_RF_2u3 = 20,      ///< referenze intensity channel 2 and 3
            LH_REF_RF_4u5,      ///< referenze intensity channel 4 and 5

        };

        // lowlevel access

        static uint32_t getOffset(lh_t entry)
        {
            return entry * sizeof(uint32_t);
        }

        /// pointer to start of buffer
        char* buf() const                           { return m_lh;   }

        /// lineheadersize
        uint32_t lhsz() const                            { return m_lhsz; }   ///< size of lineheader (bytes)

        /// get value
        uint32_t operator [] ( uint32_t idx ) const { if(idx*sizeof(uint32_t)< (unsigned int) m_lhsz) return *(uint32_t*)&m_lh[idx*sizeof(uint32_t)]; return 0; } ///< get item

        ///
        const char* ptr( int byteoffset )  const    { return m_lh+byteoffset; } ///< get char pointer with offset

        template<class valueT>
        bool hasVal(size_t offset) const
        {
            return offset + sizeof(valueT) <= m_lhsz;
        }

        template<class valueT>
        void setVal(size_t byteoffset, valueT v)
        {
            if (hasVal<valueT>(byteoffset))
                *reinterpret_cast<valueT*>(m_lh + byteoffset) = v;
        }

        template<class valueT>
        valueT getVal(size_t byteoffset) const
        {
            if (hasVal<valueT>(byteoffset))
                return *reinterpret_cast<valueT*>(m_lh + byteoffset);
            return {};
        }

        // highlevel

        bool hasId() const                     { return m_lhsz>=4;            }
        uint32_t id() const                    { return (*this)[LH_ID];       } ///< get flags
        void setId(uint32_t v = ID_LINEHEADER) { setVal(getOffset(LH_ID), v); }

        /// PC time added by software
        bool hasTime() const     { return m_lhsz>=32;             }
        uint32_t time() const    { return (*this)[LH_TIME];       }
        void setTime(uint32_t v) { setVal(getOffset(LH_TIME), v); }

        /// linenumber
        bool hasLinenumber() const     { return m_lhsz>=36;                   }
        uint32_t linenumber() const    { return (*this)[LH_LINENUMBER];       }
        void setLinenumber(uint32_t v) { setVal(getOffset(LH_LINENUMBER), v); }

        /// user defined value
        bool hasUser() const     { return m_lhsz>=40;             }
        uint32_t user() const    { return (*this)[LH_USER];       }
        void setUser(uint32_t v) { setVal(getOffset(LH_USER), v); }

        /// flags see ZFSFLAG_... in flags.h
        bool hasFlags() const     { return m_lhsz >= 24;                                   }
        int flags() const         { return getVal<uint16_t>(getOffset(LH_REF_FLAGS) + sizeof(uint16_t)); }
        void setFlags(uint16_t v) { setVal(getOffset(LH_REF_FLAGS) + sizeof(uint16_t), v); }

        bool hasRefRg() const     { return hasVal<uint16_t>(getOffset(LH_REF_FLAGS));               }
        double refRg() const      { return getVal<uint16_t>(getOffset(LH_REF_FLAGS)) / 10.;         }
        void setRefRg(double v)   { setVal(getOffset(LH_REF_FLAGS), static_cast<uint16_t>(v * 10)); }

        /// Test for Differential reference distance
        bool hasRefRgd() const    { return hasVal<int16_t>(getOffset(LH_REF_REF_RF)); }
        /// Set differential reference distance [1/10 mm]
        double refRgd() const     { return getVal<int16_t>(getOffset(LH_REF_REF_RF)) / 10.;        }
        /// Get differnetial reference distance [1/10 mm]
        void setRefRgd(double v)  { setVal(getOffset(LH_REF_REF_RF), static_cast<int16_t>(v * 10));}

        /**
         * @brief Test wether reference intensity of channel exists
         * @param ch channel number
         * @return true if channel exists (LH large enough to hold value)
         */
        bool hasRefRf(size_t ch = 0);

        /**
         * @brief Extract reference intensity of channel
         * @param ch channel number
         *
         * @note RF value is stored using a factor as specified by ZFS header
         *
         * @return intensity value if value exists or 0 if not existant
         */
        uint16_t refRf(size_t ch = 0) const;

        /**
         * @brief Write reference intensity of channel into line header
         *
         * Is a no-op if line header isn't large enough for this value
         *
         * @param v  Intensity to write into header
         * @param ch Channel to write intensity to
         */
        void setRefRf(uint16_t v, size_t ch = 0);

        /// vertical encoder
        bool hasEncVert() const    { return m_lhsz>=8;                }
        int encVert() const        { return (*this)[LH_ENC_V1];       } ///< return encoder increment of current line.
        void setEncVert(int32_t v) { setVal(getOffset(LH_ENC_V1), v); }

        /// horizontal encoder
        bool hasEncHorz() const    { return m_lhsz>=12;               }
        int encHorz() const        { return (*this)[LH_ENC_H1];       } ///< return encoder increment of current line.
        void setEncHorz(int32_t v) { setVal(getOffset(LH_ENC_H1), v); }

        /// Counters (very accurate)
        bool hasCnt1() const     { return m_lhsz>=48;             }
        uint32_t cnt1() const    { return (*this)[LH_CNT1];       }
        void setCnt1(uint32_t v) { setVal(getOffset(LH_CNT1), v); }
        bool hasCnt2() const     { return m_lhsz>=52;             }
        uint32_t cnt2() const    { return (*this)[LH_CNT2];       }
        void setCnt2(uint32_t v) { setVal(getOffset(LH_CNT2), v); }
        bool hasCnt3() const     { return m_lhsz>=56;             }
        uint32_t cnt3() const    { return (*this)[LH_CNT3];       }
        void setCnt3(uint32_t v) { setVal(getOffset(LH_CNT3), v); }

        /// level (not syncronized)
        bool hasLevel() const    { return m_lhsz>=44;                                              }
        double levelX() const    { return ((double)*(int16_t*)ptr(LH_LEVEL*4  )) / 1000.;          }
        double levelY() const    { return ((double)*(int16_t*)ptr(LH_LEVEL*4+2)) / 1000.;          }
        void setLevelX(double v) { setVal(getOffset(LH_LEVEL), int16_t(v * 1000));                 }
        void setLevelY(double v) { setVal(getOffset(LH_LEVEL) + sizeof(int16_t), int16_t(v*1000)); }

        /// GPS (not syncronized)
        bool hasGps() const      { return m_lhsz>=72;          }
        double gpsLat() const    { return *(double*)ptr(LH_GPS_LAT_low*4);   }
        double gpsLong() const   { return *(double*)ptr(LH_GPS_LONG_low*4);   }
        void setGpsLat(double v) { setVal(getOffset(LH_GPS_LAT_low), v); }
        void setGpsLon(double v) { setVal(getOffset(LH_GPS_LONG_low), v); }

        /// debug
        bool hasDbg() const { return m_lhsz >= 80; }
        uint64_t dbg() const { return *(uint64_t*)ptr(LH_DEBUG_low * 4); }
        void setDbg(uint64_t v) { setVal(getOffset(LH_DEBUG_low), v); }

        /// Pixel counter
        bool hasPixelCounter() const { return m_lhsz >= ((LH_PIXEL_COUNTER+1)*4); }
        uint32_t pixelCounter() const { return (*this)[LH_PIXEL_COUNTER]; }
        void setPixelCounter(uint32_t v) { setVal(getOffset(LH_PIXEL_COUNTER), v); }

    protected:
        char* m_lh;			///< pointer to lineheader
        uint32_t  m_lhsz;        ///< size of lineheader
    };

    //=======================================================================================================

    /**
        LineHeader with its own bufferhandling
    */
    class LineHeaderBuf : public LineHeader
    {
    public:
        LineHeaderBuf( int sz )
            : LineHeader()
        {
            alloc( sz );
        }

        LineHeaderBuf( LineHeader& lh )
            : LineHeader()
        {
            char* src = lh.buf();
            alloc( lh.lhsz() );
            if (lh.lhsz())
                memcpy(m_lh, src, lh.lhsz());
        }

        ~LineHeaderBuf()
        {
            free();
        }

        char* buf() { return (char*)m_lh; } ///< for write access as well
        bool setByOffset(uint32_t byteoffset, uint32_t val ) { if(byteoffset+(int)sizeof(uint32_t)<m_lhsz ) { *(uint32_t*)(m_lh+byteoffset) = val; return true; } return false; }
        bool setByIdx(uint32_t idx, uint32_t val ) { if((idx+1)*(uint32_t)sizeof(uint32_t)<m_lhsz ) { *(uint32_t*)(m_lh+idx*sizeof(uint32_t)) = val; return true; } return false; }

    protected:
        void alloc(int sz)
        {
            free();
            if (sz)
            {
                m_lhsz = sz;
                m_lh = new char[m_lhsz];
            }
        }
        void free()
        {
            delete [] m_lh;
            m_lh = nullptr;
        }

    };

    //=======================================================================================================

    /// get a string of the flags field from the scan class (without translation)
    ZFS_API std::wstring ZfsFlags2WString( zfs_flags_t flags );

    /// get a string of the flags field from the scan class (without translation)
    ZFS_API std::string ZfsOpenFlags2String(int openflags);

    /// gets a stringdescription of the flags in the lineheader (without translation)
    ZFS_API std::string LineheaderFlag2String(uint32_t flag);


    /// get a string description of the lineheader index (without translation)
    ZFS_API const char* Lineheader2String(uint32_t idx);

    /// convert the value(s) of a header entry to a string
    ZFS_API std::string LineheaderValue2String(const LineHeader&, uint32_t idx);

}// end namespace zfs
