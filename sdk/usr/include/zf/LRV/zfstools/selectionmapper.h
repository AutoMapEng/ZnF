#ifndef SELECTIONMAPPER_H
#define SELECTIONMAPPER_H

#include <vector>

#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <QPoint>
SWITCH_WARNINGS_ON

#include "ZfsToolsAPI.h"



namespace zfstools
{
    class MapAreaRect;
    class PolyLine;
    class PolyLines;
    
    /**
    \brief The selection mapper solves the problem of mapping selections from one "piece wise continuous" space to another.
    
    This problem occurs when one wants to map selections from FlatView space to PanoramaImage space where the images are
    separated into rectangles in which to points can be mapped with one linear function. For normal points this mapping
    is done with the PanoramaMapper and its derivatives, but with selections we have the additional problem that we need
    to map points on the right or lower border of a MapAreaRect with the same mapping function that is used inside of
    the MapAreaRect.
    The mapping function provided by the selection can map PolyLines that have points on the lower and right border of
    a MapAreaRect.
    To implement a concrete child class of the SelectionMapper you only need to provide a function that maps from the
    "home space" of the mapper to the other space and a vector of MapAreaRects in the home space of the mapper.
    The SelectionMapper only maps in one direction, so if you want the inverse mapping you need a second implementation
    of the SelectionMapper.

\internal

    The problem is solved by shifting the critical points into the rect before mapping and shifting them back out after
    the mapping. 


\warning

    Selection points that lie on the lower or right border of map area rects with widht or height == 1
    will not get correctly mapped because the shift direction of these points can not be correctly
    determined with the current algorithm. 
    At this point I do not know a solution to the problem that does not know about the internals
    of the function that maps a "normal" point.
    */
    class ZFSTOOLS_API SelectionMapper
    {
    private:
        
        std::vector<MapAreaRect> mapRects;  ///< The rectangles in this space where the points can be mapped with one linear mapping function.
        

        double scaleToOtherSpace;              ///< The scale factor of the mapping function. A value of 0.25 means that a delta of one pixel in this space means a delta of 0.25 pixel in the other space.
        
        enum BorderStatus
        {
            NO_BORDER,
            TOP,
            RIGHT,
            BOTTOM,
            LEFT
        };

        enum DeltaDirectionAfterMapping
        {
            HORIZONTAL_PARALLEL,
            HORIZONTAL_ANTI_PARALLEL,
            VERTICAL_PARALLEL,
            VERTICAL_ANTI_PARALLEL
        };

        /**
        Helper class to shorten argument lists.
        */
        class SelectionPoint : public QPoint
        {
        public:    
            SelectionPoint( QPoint point);

            BorderStatus verticalBorderStatus;
            BorderStatus horizontalBorderStatus;
            bool fromHorizontalParallel;    // if false a delta in x in one space leads to a negative delta (x or y) in the other space
            bool fromVerticalParallel;      // if false a delta in y in one space leads to a negative delta in the other space
            bool swapAxes;                  // if true a delta in x in one space leads to a delta in y in the other space
            int mapRectIndex;               // The index of the MapAreaRect int SelectionMapper::mapRects to which the point belongs.

            QPoint premapShiftDelta;
            QPoint postmapShiftDelta;
        };

    public:  
        SelectionMapper( std::vector<MapAreaRect> mapRects, double scaleToOtherSpace);
        SelectionMapper( MapAreaRect mapRect, double scaleToOtherSpace);
        virtual ~SelectionMapper(){}

        void mapSelectionToOtherSpace( const PolyLine& polygonInHomeSpace, PolyLine& polygonInOtherSpace);
        void mapSelectionToOtherSpace( PolyLine& polyLine);
        void mapSelectionsToOtherSpace( const PolyLines& polygonsInHomeSpace, PolyLines& polygonsInOtherSpace);

        std::vector<MapAreaRect> getMapRects(){ return mapRects;}

    protected:
        /**
        Delegate this call to the mapping function for your concrete space.
        */
        virtual void mapPointToOtherSpace( QPoint& point) = 0;

    private:
        QPoint mapSelectionPointToOtherSpace( SelectionPoint selectionPoint, QPointF centerOfMassOfSelection);
        void getBorderStatesOfPointAnMapAreaRectIndex(QPointF centerOfMassOfSelection, SelectionPoint& point);
        bool pointLiesOnLowerMapRectBorder(SelectionPoint& point, QPointF centerOfMass);
        bool pointLiesOnRightMapRectBorder(SelectionPoint& point, QPointF centerOfMass);
        bool pointLiesOnUpperMapRectBorder(SelectionPoint& point, QPointF centerOfMass);
        bool pointLiesOnLeftMapRectBorder(SelectionPoint& point, QPointF centerOfMass);
        void getAfterMappingPointExpansionParameter( SelectionPoint& selectionPoint);
        int getDeltaStepSizeInThisSpaceToGetStepsizeOneInOtherSpace();
        QPoint getDeltaInComplementarySpace(QPoint firstPoint, QPoint secondPoint);

        void getPreMapShiftDelta( SelectionPoint& selectionPoint);
        void getPostMapShiftDelta( SelectionPoint& selectionPoint);
        QPoint getPostMapDeltaForShiftedBorderPoints( SelectionPoint selectionPoint);
        QPoint getPostMapDeltaForMirroredAxis(SelectionPoint selectionPoint);


    };



}   // end namespace

#endif // !SELECTIONMAPPER_H