#pragma once

#include <string>
#include <vector>
#include <deque>
#include <cstdint>

#include <zfs/Math/auxilliary/types.h>
#include <zfs/Math/math/range.h>
#include <zfs/Math/3d/point_nd.h>

#include "filterapi.h"
#include "Matrix.h"
#include "basic/all.h"

class ZFSLinebyline;
class ZFScan;
class ZFScanZFS;

namespace zfsfilter
{
    //=====================================================================
    struct FilterPixel
    {

        enum class state_t :uint8_t
        {
            ORGINAL,
            FILTERED_OUT,
            MODIFIED,
            MODIFIED2
        };

        float   rg;    // < range [m]
                
        float   x;     // < x position [m]
        float   y;     // < y position [m]
        float   z;     // < z position [m]
                
        int     rf;    // < reflection [inc]
        state_t maske; // < mask value

        FilterPixel() : rg(0), x(0), y(0), z(0), rf(0), maske(state_t::ORGINAL)
        {}
    };
    //=====================================================================
    typedef std::vector<FilterPixel> filter_line_t;
    //=====================================================================
    class FilterLine
    {
    public:
        int           m_pixel;
        int           m_enc_first_pixel;
        filter_line_t m_line;

        FilterLine() : m_pixel(0), m_enc_first_pixel(0) {}
        FilterLine(int pixel) : m_pixel(pixel), m_enc_first_pixel(0) { m_line.reserve(pixel); }

        FilterPixel& operator[] (int px) { if (px < m_pixel) return m_line[px]; return m_line[0]; }
        FilterPixel  operator[] (int px) const { if (px < m_pixel) return m_line[px]; return m_line[0]; }
    };
    //=====================================================================
    // to avoid Warning: 4251: see also: http://support.microsoft.com/kb/168958/en-us

    typedef std::deque<FilterLine> RBufStorage_Type;
    //=====================================================================
    enum class RB_SCANNER_TYPE:uint8_t { RB_SCANNER_UNDEF = 0, RB_SCANNER_IMAGER = 1, RB_SCANNER_PROFILER = 2 };
    //=====================================================================
    /** \class RollingBuffer
     * \brief  Class managing data (only reading operations) from a zfs file in a rolling buffer like fashion.
     *
     * \author Juergen Holzner
     *
     * Class managing data (only reading operations) from a zfs file in a rolling buffer like fashion.
     *
     */
    class FILTERDLL_API RollingBuffer
    {
    public:
        // options
        enum { RB_AUTO = -1 };

        // call back function type to provide the xyz information for profiler data (via navigation and calibration information)
        typedef bool(*cb_GetProfilerXYZLine_f)(int line, int pixel_start, int pixel_end, zfsfilter::FilterLine& DataLine, zfs::math::PointND<float, 3>& CenterPoint);

    protected:
        bool m_storeXYZ;            //true = load xyz Data fore eache Point 

        // member variables
        ZFSLinebyline*            m_zfLine;                            ///< der Scan (LineByLine)
        ZFScanZFS*                m_zfScan;                            ///< der Scan (ZFScan)

        bool                      m_localOpen;                         ///< info ob local geoeffnet oder uebergeben
        bool                      m_externalOpen;

        bool                      m_profiler_scan;                     ///< indicate if profiler scan

        bool                      m_use_zfScan;                        ///< input file option (LineByline or zfScan)
        bool                      m_eof_reached;                       ///< end of file reached flag
        bool                      m_file_open_success;                 ///< file open successful?

        bool                      m_overlap_flag;                      ///< overlap of loaded image portions

        bool                      m_load_xyz;                          ///< indicates if xyz data has to be loaded

        int                       m_curAccessMode;                     ///< stores the current access mode given ZFScan ( which is switched to M_PHYS)
        int                       m_scan_pixels, m_scan_lines;         ///< physical
        int                       m_pixel_start, m_pixel_end;          ///< physical/user
        int                       m_line_start, m_line_end;            ///< physical/user
        int                       m_number_of_lines_to_process;        ///< number of lines to process
        int                       m_buffer_pixels, m_buffer_lines;     ///< der rolling buffer
        int                       m_no_of_tiles, m_tile_count;         ///< tiling of data
        int                       m_file_valid_start, m_file_valid_end;///< tiling: valid section in file
        int                       m_tile_valid_start, m_tile_valid_end;///< tiling: valid section in tile
        int                       m_overlap;                           ///< tiling: overlap
        int                       m_reload_size;                       ///< stepsize
        int                       m_open_single;                       ///< data set opened in Single Mode
        int                       m_current_scan_line;                 ///< current scan line HasNextLine() doesn't stop at data set end.
        int                       m_start_line_current_file_section;   ///< start line of current file section;

        double                    m_profile_feed;                      ///< in profiler mode and when cb_GetProfilerXYZLine_f=NULL a constant feed can be used for init of the data

        std::wstring              m_scan_file_path;                    ///< path to scan

        // error status
        ferr_t                    m_error_status;

        // data storage
        RBufStorage_Type          m_Buffer;                            ///< data storage

        // call back function to provide the xyz information for profiler data (via navigation and calibration information)
        std::deque<zfs::math::PointND<float, 3>>  m_ProfilerCenterPositions;
        cb_GetProfilerXYZLine_f                m_GetProfilerXYZLineFunction;

        // private member functions
        void calculateNumberOfTiles();
        void calculateValidSectionStartEnd();

    public:

        RollingBuffer();
        virtual ~RollingBuffer();

        // -- initialization

        void            basic_initialization();

        ZFSLinebyline*  initialize(const std::wstring& file_path, int BufferLines, int ReloadSize, int LineStart, int LineEnd, int PixelStart, int PixelEnd, bool bLoadXYZ= false, const double& a_ProfileFeed= 0., cb_GetProfilerXYZLine_f a_GetProfilerXYZLineFunction = NULL); /// < possibility to open a given a file name (uses ZFLinebyLine)
        void            initialize(ZFSLinebyline* z, int BufferLines, int ReloadSize, int LineStart, int LineEnd, int PixelStart, int PixelEnd, bool bLoadXYZ= false, const double& a_ProfileFeed= 0., cb_GetProfilerXYZLine_f a_GetProfilerXYZLineFunction = NULL);              /// < possibility to pass ZFLinebyLine
//        void            initialize( ZFScan* z, int BufferLines, int ReloadSize, int LineStart, int LineEnd, int PixelStart, int PixelEnd, bool bLoadXYZ= false, const double& a_ProfileFeed = 0., cb_GetProfilerXYZLine_f a_GetProfilerXYZLineFunction = NULL );                     /// < possibility to pass ZFScan

        void loadXYZ( bool b ) { m_storeXYZ = b; }
        // -- file access/ operations
        bool FileOpenOK();
        void Reset();

        bool LoadFirstDataPortionZFLinebyline();
        bool LoadFirstDataPortionZFScan();

        bool LoadNextDataPortionZFLinebyline(int SkipLinesAbsoluteFromFileStart = -1);// if SkipLinesAbsoluteFromFileStart=-1 no skipping, sequential reload of tiles
        bool LoadNextDataPortionZFScan(int SkipLinesAbsoluteFromFileStart = -1);      // if SkipLinesAbsoluteFromFileStart=-1 no skipping, sequential reload of tiles

        void CloseFile();
        bool EOFReached();

        void ResetZFScanToOriginalAccessMode();

        // -- access to values in buffer
        // the indices pixel and line
        // refer to indices within buffer not
        // within file!
        float                        GetRgm(int pixel, int line) const;
        float                        GetX(int pixel, int line) const;
        float                        GetY(int pixel, int line) const;
        float                        GetZ(int pixel, int line) const;
        int                          GetRf(int pixel, int line) const;
        FilterPixel::state_t         GetMask(int pixel, int line) const;
        const zfs::math::Point3D<float> GetPos( int pixel, int line ) const; // (X,Y,Z) as Point3D<float>()
        const FilterPixel            GetPixel(int pixel, int line) const;

        void SetX( int pixel, int line, float xVal );
        void SetY( int pixel, int line, float yVal );
        void SetZ( int pixel, int line, float zVal );
        void SetMask( int pixel, int line, FilterPixel::state_t mask );
        void SetRf( int pixel, int line, int inc );
        void SetRgm( int pixel, int line, float rg );

        // a factor can be given to distance convert values, e.g. from m to mm
        void            FillRfMatrix(Matrix<int>   & Rf, int line_start = 0, int line_end = -1) const;
        void            FillProfPosXYZ(std::vector<zfs::math::PointND<float, 3>>& ProfilerPosVector, int line_start = 0, int line_end = -1) const;

        void FillMaskMatrix( Matrix<bool>& Mask, int line_start = 0, int line_end = -1) const;

        void            FillEncValFirstPixelsVector( std::vector<int>& EncValFirstPixels, int line_start = 0, int line_end = -1) const;

        template<class T> void FillRgMatrix   ( Matrix<T>& Rg, int line_start = 0, int line_end = -1, const T& factor = T(1)) const;
        template<class T> void FillXMatrix    ( Matrix<T>&  X, int line_start = 0, int line_end = -1, const T& factor = T(1)) const;
        template<class T> void FillYMatrix    ( Matrix<T>&  Y, int line_start = 0, int line_end = -1, const T& factor = T(1)) const;
        template<class T> void FillZMatrix    ( Matrix<T>&  Z, int line_start = 0, int line_end = -1, const T& factor = T(1)) const;
        template<class T> void FillPointMatrix( MatrixBase< zfs::math::Point3D< T > >&  Pos, int line_start = 0, int line_end = -1, bool bExchangeXY= false) const;

        // -- getter functions
        // extend and relative position in file
        int             BufferPixels() const;
        int             BufferLines() const;
        int             BufferWidth() const;

        int             ScanPixels() const;
        int             ScanLines() const;

        // getters for overlap processing
        // - subsequent tiles overlap each other - except first and last tile
        // - the valid sections within each tile are contiguous (check input parameter setting!)
        // - overlap ensures contiguous processing behaviour
        int             TileCount() const;
        int             NumberOfTiles() const;

        int             ValidSectionStart() const;
        int             ValidSectionEnd() const;
        int             ValidBufferSectionStart() const;
        int             ValidBufferSectionEnd() const;

        int             getPixelStart() const;
        int             getPixelEnd() const;

        int             getLineStart() const;
        int             getLineEnd() const;

        int             getNoOfLinesToProcess() const;

        int             getStartLineOfCurrentFileSection() const;
        bool            getOverlapExists() const;

        // file status
        bool            getLocalOpen() const;

        // error status
        ferr_t          getErrorStatus() const;

        // extract parameter values from scan header
        long            getLongScanParameter(int PNum) const;
        double          getDoubleScanParameter(int PNum) const;
        std::string     getCStringScanParameter(int PNum) const;
        std::wstring    getWStringScanParameter(int PNum, std::wstring& def) const;
        // retrieve scanner type from file header
        RB_SCANNER_TYPE getScannerType() const;

        // get scan
        ZFSLinebyline* getScanLineByLine();
        ZFScanZFS*     getScan();

        // get center positions in case profiler data with valid cabib data is processed
        std::vector<zfs::math::PointND<double, 3>> getCenterPositionsOfProfiler() const;
    };
    //=====================================================================
} // namespace zfsfilter
