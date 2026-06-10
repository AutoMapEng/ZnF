#pragma once

#include <string>
#include <cstdint>

#include <zfs/Math/3d/point_nd.h>

#include "filterapi.h"
#include "basic/all.h"
#include "datasetparam.h"
#include "basicfilter.h"

class ZFLineByLineWrite;
class ZFScan;
class ZFLine;
class ZFLineInfo;
class ZFScanList;

namespace zfs
{
    namespace math
    {
        template<class T> class Point3D;
    }
}

namespace zfsfilter
{
//#define EN_DEBUG_OUTPUT

    // ============================================================================================================
    /** \class TileFilter
     * \brief  Base class for tile based operating filters (Mixed Pixel Filter, Road Sign Filter).
     *
     * \author Juergen Holzner
     *
     * Base class for tile based operating filters (Mixed Pixel Filter, Road Sign Filter).
     *
     */
    class FILTERDLL_API TileFilter : virtual public BasicFilter
    {
      protected:

        // -- member variables

        // bool indicators
        bool         m_CurrentTileIsLastTile;          ///< Indicates if current tile is last tile

        // data set data
        int32_t      m_NumberOfTilesToProcess;         ///< Number of tiles to process
        int32_t      m_NumberOfScanLines=0;              ///< Number of scan lines
        int32_t      m_NumberOfScanPixels=0;             ///< Number of scan pixels
        std::wstring m_InputDataSetName;               ///< Input data set name
        std::wstring m_OutputDataSetName;              ///< Output data set name


        // buffer/tile management
        int32_t      m_TileSize;                       ///< Size of tiles used for processing (Standard Size)
        int32_t      m_CurrentTileSize;                ///< Size of tiles used for processing (e.g. important for last tile!)
                     
        int32_t      m_Overlap;                        ///< Overlap size used
        int32_t      m_ReloadSize;                     ///< Reload size of the buffer
        int32_t      m_StartLineCurrentFileSection;    ///< Start line in the file of the data currently in the buffer
        int32_t      m_TileCounter;                    ///< Counter for tiles
        int32_t      m_ValidStart;                     ///< Keeps the start line from which valid data is kept in the matrices (excluding overlap area)
        int32_t      m_ValidEnd;                       ///< Keeps the end line from which valid data is kept in the matrices (excluding overlap area)
        int32_t      m_ValidBufferStart;               ///< Keeps the start line from which valid data is kept in the rolling buffer (excluding overlap area)
        int32_t      m_ValidBufferEnd;                 ///< Keeps the start line from which valid data is kept in the rolling buffer (excluding overlap area)


        // -- member functions
        void basic_initialization();

        // -- file writing helper
		bool write_line_to_file_from_ZFScan( ZFLineByLineWrite* writeZfs, ZFLine* ZFLine, ZFLineInfo* a_ZFInfo );

		// -- debugging helper function
#ifdef EN_DEBUG_OUTPUT

		template< class T> bool writeMatrixToDisk( const std::wstring& aFilenameTemplate, const uint32_t aTileNumber, const Matrix<T>& aMatrixToWrite ) const;
		template< class T> bool writeMatrixToDisk( const std::wstring& aFilenameTemplate, const uint32_t aTileNumber, const uint32_t aAddCounter, const Matrix<T>& aMatrixToWrite ) const;

        template< class T> bool writeVectorToDisk( const std::wstring& aFilenameTemplate, const uint32_t aTileNumber, const std::vector<T>& aVectorToWrite ) const;
        template< class T> bool writeVectorToDisk( const std::wstring& aFilenameTemplate, const uint32_t aTileNumber, const uint32_t aAddCounter, const std::vector<T>& aVectorToWrite ) const;

		template< class T> bool writeMatrixToDisk( const std::wstring& aFilenameTemplate, const uint32_t aTileNumber, const MatrixBase<zfs::math::Point3D<T>>& aMatrixToWrite ) const;
		template< class T> bool writeMatrixToDisk( const std::wstring& aFilenameTemplate, const uint32_t aTileNumber, const uint32_t aAddCounter, const MatrixBase<zfs::math::Point3D<T>>& aMatrixToWrite ) const;
        
        template< class T> bool writeVectorToDisk( const std::wstring& aFilenameTemplate, const uint32_t aTileNumber, const std::vector<zfs::math::Point3D<T>>& aVectorToWrite ) const;
        template< class T> bool writeVectorToDisk( const std::wstring& aFilenameTemplate, const uint32_t aTileNumber, const uint32_t aAddCounter, const std::vector<zfs::math::Point3D<T>>& aVectorToWrite ) const;
#endif

    public:

        // --       constructor (for init only)
        TileFilter();

        TileFilter( const std::wstring& a_DataSetName);

        TileFilter( const std::wstring& a_DataSetName,
                    const bool          a_bWriteDebugFiles,
                    const std::wstring& a_DebugFileDirectory);

        TileFilter( const std::wstring& a_DataSetName,
                    const int32_t       a_LineStart,
                    const int32_t       a_LineEnd,
                    const int32_t       a_PixelStart,
                    const int32_t       a_PixelEnd,
                    const bool          a_bOutputReport);

        TileFilter( const std::wstring& a_DataSetName,
                    const int32_t       a_LineStart,
                    const int32_t       a_LineEnd,
                    const int32_t       a_PixelStart,
                    const int32_t       a_PixelEnd,
                    const bool          a_bOutputReport,
                    const bool          a_bWriteDebugFiles,
                    const std::wstring& a_DebugFileDirectory);

        TileFilter( const std::wstring& a_DataSetName,
                    const int32_t       a_LineStart,
                    const int32_t       a_LineEnd,
                    const int32_t       a_LineSubsample,
                    const int32_t       a_PixelStart,
                    const int32_t       a_PixelEnd,
                    const int32_t       a_PixelSubsample,
                    const bool          a_bOutputReport);

        TileFilter( const std::wstring& a_DataSetName,
                    const int32_t       a_LineStart,
                    const int32_t       a_LineEnd,
                    const int32_t       a_LineSubsample,
                    const int32_t       a_PixelStart,
                    const int32_t       a_PixelEnd,
                    const int32_t       a_PixelSubsample,
                    const bool          a_bOutputReport,
                    const bool          a_bWriteDebugFiles,
                    const std::wstring& a_DebugFileDirectory);


        virtual ~TileFilter();

        // -- getter
        // -- member value retrieval
        bool          IsCurrentTileLastTile() const;

        int32_t       getNumberOfTilesToProcess() const;
        int32_t       getTileCounter() const;
        int32_t       getValidStart() const;
        int32_t       getValidEnd() const;
        int32_t       getValidBufferStart() const;
        int32_t       getValidBufferEnd() const;
        int32_t       getTileSize() const;
        int32_t       getOverlap() const;
        int32_t       getReloadSize() const;
        int32_t       getStartLineCurrentFileSection() const;
        std::wstring  getInputDataSetName() const;
        std::wstring  getOutputDataSetName() const;
        // -- additional information
        int32_t       getLengthOfValidSection() const;

        // -- setter
        void          setInputDataSetName(const std::string& a_DatasetName);
        void          setInputDataSetName(const std::wstring& a_DatasetName);
        void          setOutputDataSetName(const std::string& a_DatasetName);
        void          setOutputDataSetName(const std::wstring& a_DatasetName);
		void          setTileCounter( const int32_t aTileNumber );

        // -- calculators
        void          CalculateNumberOfTiles();

        // -- logging
        bool          OpenLogFile( std::string aLogFileNameTemplate, const int32_t aLogFileNumber, std::string aLogPrefix);

        //virtual ~TileFilter(){}
    };
    // ============================================================================================================


        // ============================================================================================================
    // TileFilter: getter
    // ============================================================================================================
    // ============================================================================================================
    inline int32_t TileFilter::getLengthOfValidSection() const
    {
        return m_ValidBufferEnd - m_ValidBufferStart;
    }
    // ============================================================================================================
    inline bool TileFilter::IsCurrentTileLastTile() const
    {
        return m_CurrentTileIsLastTile;
    }
    // ============================================================================================================
    inline int32_t TileFilter::getNumberOfTilesToProcess() const
    {
        return m_NumberOfTilesToProcess;
    }
    // ============================================================================================================
    inline int32_t TileFilter::getTileCounter() const
    {
        return m_TileCounter;
    }
    // ============================================================================================================
    inline int32_t TileFilter::getValidStart() const
    {
        return m_ValidStart;
    }
    // ============================================================================================================
    inline int32_t TileFilter::getValidEnd() const
    {
        return m_ValidEnd;
    }
    // ============================================================================================================
    inline int32_t TileFilter::getValidBufferStart() const
    {
        return m_ValidBufferStart;
    }
    // ============================================================================================================
    inline int32_t TileFilter::getValidBufferEnd() const
    {
        return m_ValidBufferEnd;
    }
    // ============================================================================================================
    inline int32_t TileFilter::getTileSize() const
    {
        return m_TileSize;
    }
    // ============================================================================================================
    inline int32_t TileFilter::getOverlap() const
    {
        return m_Overlap;
    }
    // ============================================================================================================
    inline int32_t TileFilter::getReloadSize() const
    {
        return m_ReloadSize;
    }
    // ============================================================================================================
    inline int32_t TileFilter::getStartLineCurrentFileSection() const
    {
        return m_StartLineCurrentFileSection;
    }
    // ============================================================================================================
    inline std::wstring TileFilter::getInputDataSetName() const
    {
        return m_InputDataSetName;
    }
    // ============================================================================================================
    inline std::wstring TileFilter::getOutputDataSetName() const
    {
        return m_OutputDataSetName;
    }
    // ============================================================================================================
    // TileFilter: setter
    // ============================================================================================================
    inline void TileFilter::setInputDataSetName(const std::wstring& a_DatasetName)
    {
        m_InputDataSetName = a_DatasetName;
    }
    //==========================================================================================================================================
    inline void TileFilter::setInputDataSetName(const std::string& a_DatasetName)
    {
        m_InputDataSetName = std::wstring(a_DatasetName.begin(), a_DatasetName.end());
    }
    // ============================================================================================================
    inline void TileFilter::setOutputDataSetName(const std::wstring& a_DatasetName)
    {
        m_OutputDataSetName = a_DatasetName;
    }
    //==========================================================================================================================================
    inline void TileFilter::setOutputDataSetName(const std::string& a_DatasetName)
    {
        m_OutputDataSetName = std::wstring(a_DatasetName.begin(), a_DatasetName.end());
    }
    //==========================================================================================================================================
    inline void TileFilter::setTileCounter(const int32_t aTileNumber)
    {
        m_TileCounter = aTileNumber;
    }

} // namespace zfsfilter
