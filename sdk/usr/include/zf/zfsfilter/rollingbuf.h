#pragma once


#include <algorithm>
#include <vector>
#include <deque>
#include <memory>
#include <string>
#include <cstdint>

#include <zfs/Math/3d/point_nd.h>

#include <zfs/zfs.h>
#include <zfs/Math/zfvec.h>
#include <zfs/pixel.h>

#include "filterapi.h"

#undef max
#undef min

namespace zfs {
    /**
        a single pre calculated point in the rollingbuffer
    */
#pragma pack(1)
    class FILTERDLL_API ZFPt : public zfs::PDataRGB
    {

    public:
        enum {     ///< values
                   V_RGB        = 1,
                   V_RF1        = 2,
                   V_ANGLE      = 4,
                   V_XYZ        = 8,
                   V_MASK       = 0x10,
                   V_ADD        = 0x20,
                   V_RAW        = 0x40,            ///< raw values available
                   V_LINEHEADER = 0x80             ///< raw lineheader
              };

        std::vector<uint8_t> raw;                  // raw

        bool    valid;                             ///< if point is valid
        int8_t  add = 0;                           ///< Additional byte V_ADD

                                                   // --- weitere berechnete Werte
        double vert=0., horz=0.;                   ///< angle [rad] V_ANGLE

                                                   
        uint16_t mask{ 0 };                 // result                

        ZFPt(bool v = false, int f = 0, double r = 0.) 
            : PDataRGB(),
            valid(v)
            {}
    };
#pragma pack()

    typedef std::vector<ZFPt> zfpts_t;

    /**
        a whole pre calculated scanline inclusive lineheader
    */
    class FILTERDLL_API ZFPtLine
    {
    public:
        int                        id;                ///< linenumber or ID
        int                        thread_idx;        ///< index of the calculating thread (for multithreaded processing with filter chain)
        int                        values;            ///< what values are in ZFPtLine
        zfpts_t                    pts;               ///< The points of the line
        std::vector<uint8_t>       lineHeader;        ///< optional the orginal lineheader
        zfs::math::PointND<double, 3> laserCenter;       ///< imager: (0,0,0) or (center_x_fix, center_y_fix, center_z_fix); profiler: (center_x_from_trajectory, center_y_from_trajectory, center_z_from_trajectory)

        ZFPtLine(int i= 0): id(i), thread_idx(0), values(0)
        {}

        ZFPtLine(int szPt, int szLH): id(0), thread_idx(0), values(0)
        {
            alloc( szPt, szLH );
        }

        ZFPtLine(int i, int thrIdx, int v): id(i), thread_idx(thrIdx), values(v)
        {}

        ZFPtLine(int i, int thrIdx, int v, int szPt, int szLH): id(i), thread_idx(thrIdx), values(v)
        {
            alloc( szPt, szLH );
        }

        void alloc( int szPt, int szLH )
        {
            if (szPt)
                pts.resize(szPt);
            if (szLH)
                lineHeader.resize(szLH);
        }

        virtual ~ZFPtLine() {}

        bool hasLineheader() const { return lineHeader.size() == 0 ? false : true; }
    };

    typedef std::shared_ptr<ZFPtLine> zfptline_t;
    typedef std::vector<zfptline_t>   zfptarray_t;
    typedef std::deque <zfptline_t>   zfptqueue_t;

    //#######################################################################################################################################
    //#######################################################################################################################################
    //#######################################################################################################################################

    /*
        Basic class for streaming data into rollingbuffer
    */
    class FILTERDLL_API RollingBufferStream
    {
    public:
        virtual ~RollingBufferStream() {}
        virtual void Open(const std::wstring&) = 0;
        virtual bool HasData() const = 0;
        virtual void GetLine(int ln, ZFPtLine& b) const = 0;
        virtual void Close() {}

        virtual int  Lines() const = 0;
        virtual int  Pixel() const = 0;

        virtual int  LineHeaderSize() const = 0;

        virtual void WriteLine(int ln, ZFPtLine& b) = 0;
    };

    /*
        streams a zfs scan via linebyline class into the rollingbuffer
    */
    class FILTERDLL_API RollingBufferLinebylineStream : public RollingBufferStream
    {
    public:

        RollingBufferLinebylineStream();
        RollingBufferLinebylineStream(ZFSLinebyline* a_lbl, int a_sub = 1);

        void Open(const std::wstring&);
        bool HasData() const;
        void GetLine(int ln, ZFPtLine& b) const;
        void Close();

        int  Lines() const;
        int  Pixel() const;

        int  LineHeaderSize() const;

        void WriteLine(int ln, ZFPtLine& b);


        void set_mask_name(std::wstring mask_name);
        void set_change_pix_value(bool bChangePixValue = false);
        void set_sub(int sub);
        void set_lbl(ZFSLinebyline* a_lbl);

        int  get_sub() const;
        int  get_pixel() const;
        int  get_ext_open() const;

        ZFSLinebyline* get_lbl() const;

    private:

        ZFSLinebyline* m_lbl;
        int  m_pixel;
        int  m_sub;
        bool m_ext_open, m_change_pix_value;
        std::wstring m_mask_name;
    };

    //#######################################################################################################################################
    //#######################################################################################################################################
    //#######################################################################################################################################

    /**
        class which can hold 3 or more lines in memory which are precalculated (xyz)
    */
    class FILTERDLL_API RollingBuffer
    {
    public:
        zfptarray_t               m_buf;        ///< die zeilen als xyz

    protected: // for access of child classes
        int                       m_roll;                   ///< anzahl zeilen vor bzw nachher
        int                       m_sub;                    ///< subsample lines
        int                       m_step;                   ///< stepping to allow overlapped blocks (m_step is the stepping on the subsampled lines: m_step*m_sub on the original lines)
        std::string               m_name;                   ///< name of filter

    private:
        bool                      m_status;

        RollingBufferStream*      m_strm;

        int                       m_nBuf;                   ///< vor, nachher + aktuell

        int                       m_pixel;                  ///< anzahl der gesubsampleten pixel im buffer
        int                       m_cur_line;               ///< current middle line points to this line

        void fill_buffer()
        {
            int line2load = 0;
            for (zfptarray_t::iterator it = m_buf.begin(); it != m_buf.end(); ++it)
            {
                m_strm->GetLine(line2load, **it);
                line2load += m_sub;
            }
        }

        void Alloc();
        void Free();
        void setCurLine();

    public:
        RollingBuffer();
        RollingBuffer(RollingBufferStream *strm, int nroll = 0, int sub = 1, int step = 1);
        virtual ~RollingBuffer();

        void Init(int nroll = 0, int sub = 1, int step = 1) { m_roll = nroll; m_sub = sub; m_step = step; Alloc(); };

        void Insert(zfptline_t);
        virtual void ProcessAll() {}
        virtual void OnLine(zfptline_t) {}
        virtual void OnFilter() {}
        virtual void Abort(){};

        /* The function which is called whenever new data are available */
        //virtual void OnHasLine( zfptarray_t& /*pre*/, ZFPtLine& /*cur*/, zfptarray_t& /*next*/ ) {}

        /* a new line via stream is available*/
        virtual void OnGetNewLine()
        {
            if (!next()) m_status = false;
        }

        /* the oldest line will be freed. Use that if you want store stream to disk */
        virtual void OnFreeLine() {}

        /**
         * @brief load the next line into the rolling buffer
         *
         * @retval false if loading failes (e.g. due to end of file)
         */
        bool next1()
        {
            /*
            const int line_to_load = m_cur_line + m_sub + (m_sub * m_roll);
            if(line_to_load < m_strm->Lines() && !m_buf.empty())
            {
                // shift all elements by one
                // element dropped at front will be recycled at end...
                ZFPtLine *line = *m_buf.begin();
                m_buf.push_back(line);
                m_buf.pop_front();

                m_strm->GetLine(line_to_load, *line);

                m_cur_line += m_sub;
                return true;
            }*/
            return false;
        }

        /**
         * @brief load the next lines into the rolling buffer
         *
         * @retval false if loading failes (e.g. due to end of file)
         */
        bool next()
        {
            /*
            const int n_lines_to_load= std::max(m_nBuf, m_step);

            int cur_line_to_load= m_cur_line + m_step*m_sub + (m_sub * (m_roll- (n_lines_to_load- 1))); // depending on step, subsampling and roll new lines are loaded

            for(int i= 0; i< n_lines_to_load; i++)
            {
                cur_line_to_load+= i* m_sub;
                if(cur_line_to_load < m_strm->Lines() && !m_buf.empty())
                {
                    // shift all elements by one
                    // element dropped at front will be recycled at end...
                    ZFPtLine *line = *m_buf.begin();
                    m_buf.push_back(line);
                    m_buf.pop_front();

                    m_strm->GetLine(cur_line_to_load, *line);
                }
                else
                {
                    return false;
                }
            }
            m_cur_line+= m_step*m_sub;
            */
            return true;
        }

        /**
         * @brief Return const reference to pre calculated line
         *
         * pos should be relative to the current line (middle line). A negative
         * pos takes lines in before the current line from the rolling buffer.
         *
         * Max value of pos is the nroll value the constructor was called with.
         *
         * @throws std::out_of_range
         * @param pos relative position to current line
         * @return const refernce to line
         */
        const ZFPtLine& at(int pos) const
        {
            if (std::abs(pos) > m_roll)
                throw std::out_of_range("pos argument to high or to low");
            return *m_buf.at(pos + m_roll);
        }

        /**
         * @brief Return reference to pre calculated line
         *
         * pos should be relative to the current line (middle line). A negative
         * pos takes lines in before the current line from the rolling buffer.
         *
         * Max value of pos is the nroll value the constructor was called with.
         *
         * @throws std::out_of_range
         * @param pos relative position to current line
         * @return refernce to line
         */
        ZFPtLine& at(int pos)
        {
            if (std::abs(pos) > m_roll)
                throw std::out_of_range("pos argument to high or to low");
            return *m_buf.at(pos + m_roll);
        }

        /**
         * @brief Mapps the internal position counting to the position in
         *        line by line file
         *
         * @param pos relative position to current line (0 -> current line)
         * @return corresponding line number in line by line
         */
        const int realat(int pos) const
        {
            if (std::abs(pos) > m_roll)
                throw std::out_of_range("pos argument to high or to low");
            return (m_cur_line + pos) * m_sub;
        }

    public:

        /*Accessors*/

        /**
         * @brief access to member
         *        for subsampling
         * @param subsampling factor
         * @return no return (void)
         */
        void set_sub(int sub)
        {
            m_sub = (sub > 0) ? sub : 1;
        }

        /**
         * @brief access to member
         *        for roll
         * @param roll
         * @return no return (void)
         */
        void set_roll(int roll)
        {
            m_roll = (roll > -1) ? roll : 0;
        }

        /**
         * @brief access to member
         *        for stepping
         * @param stepping
         * @return no return (void)
         */
        void set_step(int step)
        {
            m_step = (step > 0) ? step : 1;
        }

        /**
         * @brief access to member
         *        for cur_line
         * @return cur_line (int)
         */
        int get_cur_line() const
        {
            return m_cur_line;
        }

        /**
         * @brief access to member
         *        for name
         * @return name (std::string)
         */
        std::string Name() const
        {
            return m_name;
        }

        /**
         * @brief access to member
         *        for step
         * @return step (int)
         */
        int get_step() const
        {
            return m_step;
        }

        /**
         * @brief access to member
         *        for roll
         * @return roll (int)
         */
        int get_roll() const
        {
            return m_roll;
        }

        /**
         * @brief access to member
         *        for sub
         * @return sub (int)
         */
        int get_sub() const
        {
            return m_sub;
        }

        /**
         * @brief access to member
         *        for status
         * @return status (bool)
         */
        bool get_status() const
        {
            return m_status;
        }

        /**
         * @brief access to member
         *        for no. of lines
         * @return no. of lines
         */
        int Lines() const
        {
            return m_nBuf;
        }

        /**
         * @brief access to member
         *        for no. of pixel
         * @return no. of pixel
         */
        int Pixel() const
        {
            return m_pixel;
        }
    };


    class FILTERDLL_API FilterDummy_RollingBuffer : public RollingBuffer
    {
    public:
        FilterDummy_RollingBuffer() : RollingBuffer() { m_name = "Dummy"; }
    };

}// end namespace
