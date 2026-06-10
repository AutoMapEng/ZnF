#ifndef SELECTIONMAPPERIMPLEMENTATIONS_H 
#define SELECTIONMAPPERIMPLEMENTATIONS_H 

#include "selectionmapper.h"

#include "ZfsToolsAPI.h"

class ZFScan;

namespace zfstools
{
    class PanoramaImageToFlatViewMapper;
    class PanoramaImageToPhysicalScanMapper;
    class ContinousScanPiecesFinder;

    /**
    \brief Implementation of the SelectionMapper that maps from panorama image space to flat-view space.
    */
    class ZFSTOOLS_API PanoramaToFlatviewSingleDirectionSelectionMapper : public SelectionMapper
    {
    public:
        PanoramaToFlatviewSingleDirectionSelectionMapper( PanoramaImageToFlatViewMapper* mapper);

    protected:
        virtual void mapPointToOtherSpace( QPoint& point);

    private:
        PanoramaImageToFlatViewMapper* mapper;
    };

    /**
    \brief Implementation of the SelectionMapper that maps from flat-view space to panorama image space.
    */
    class ZFSTOOLS_API FlatviewToPanoramaSingleDirectionSelectionMapper : public SelectionMapper
    {
    public:   
        FlatviewToPanoramaSingleDirectionSelectionMapper( PanoramaImageToFlatViewMapper* mapper);

    protected:
        virtual void mapPointToOtherSpace( QPoint& point);

    private:
        PanoramaImageToFlatViewMapper* mapper;
    };

    /**
    \brief Implementation of the SelectionMapper that maps from physical scan space to panorama image space.
    */
    class ZFSTOOLS_API PhysicalScanToPanoramaSingleDirectionSelectionMapper : public SelectionMapper
    {
    public:   
        PhysicalScanToPanoramaSingleDirectionSelectionMapper( PanoramaImageToPhysicalScanMapper* mapper);

    protected:
        virtual void mapPointToOtherSpace( QPoint& point);

    private:
        PanoramaImageToPhysicalScanMapper* mapper;
    };

    /**
    \brief Implementation of the SelectionMapper that maps from flat-view space to physical scan space.
    */
    class ZFSTOOLS_API PhysicalScanToFlatViewSingleDirectionSelectionMapper : public SelectionMapper
    {
    public:   
        PhysicalScanToFlatViewSingleDirectionSelectionMapper( ZFScan* scan, std::vector<MapAreaRect> physicalScanRects);

    protected:
        virtual void mapPointToOtherSpace( QPoint& point);

    private:
        ZFScan* scan;
    };

    /**
    \brief Implementation of the SelectionMapper that maps from physical scan space to flat-view space.
    */
    class ZFSTOOLS_API FlatViewToPhysicalScanSingleDirectionSelectionMapper : public SelectionMapper
    {
    public:   
        FlatViewToPhysicalScanSingleDirectionSelectionMapper( ZFScan* scan, std::vector<MapAreaRect> flatViewRects);

    protected:
        virtual void mapPointToOtherSpace( QPoint& point);

    private:
        ZFScan* scan;
    };
}

#endif  // !SELECTIONMAPPERIMPLEMENTATIONS_H 