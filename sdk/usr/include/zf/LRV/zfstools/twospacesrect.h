#pragma once
#include <algorithm>

#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <QRect>
#include <QPoint>
SWITCH_WARNINGS_ON

#include "ZfsToolsAPI.h"

class ZFScan;


namespace zfstools
{
    class PolyLine;


/**
\brief Helper class for the TwoSpacesRect.
*/
class ZFSTOOLS_API MapAreaRect : public QRect
{
public:
    MapAreaRect(){;}
    MapAreaRect( QRect rect) : QRect(rect){;}
    MapAreaRect( QPoint upperLeft, QSize size) : QRect(upperLeft, size){;}
    MapAreaRect( QPoint upperLeft, QPoint lowerRight) : QRect(upperLeft, lowerRight){;}
    
    
    bool pointIsInRect(QPoint point) const;          ///< Returns true if the point is in the rect, or on the upper or left border. The bottom left and top right corner are not considered to be within the rect. 
    bool allPointsAreInRect( const PolyLine& polyLine) const;
    bool noPointIsInRect( const PolyLine& polyLine) const;
    bool pointIsOnRectBorder( QPoint point);

    int area() const { return abs((width()) * (height())); } ///< The number of pixels that is considered to lie within the rect.

    PolyLine getAllFourCorners();
    PolyLine getAllFourInsideCorners();  ///< Returns the corners of the rect, but the ones that lie outside of the rect are shifted one pixel to be within the rect.
    PolyLine getAllFourOutsideCorners();    ///< Returns the points at the corners that lie one point outsied of the rect.
};

/**
\brief A rectangle in two different spaces that can be mapped to each other with a linear function.

The need for this class occurs when we deal with mapping functions like the PanoramaMapper or the
ZFScan::Map() / ZFScan::UnMap() functions where the mapped spaces consist of areas that can be teselated
into rectangles for which simple linear mapping functions exist. For these functions a rect in space A
will map onto a rect in space B. TwoSpacesRect forms a base class for these cases.

\warning

We consider points on the upper and left edge to be within the rect, but not the points on the lower and right
border.
*/
class ZFSTOOLS_API TwoSpacesRect
{
public:
    MapAreaRect rectA;    ///< The rectangle in space A
    MapAreaRect rectB;    ///< The rectangle in space B

public:
    TwoSpacesRect( QRect rectA, QRect rectB);
    TwoSpacesRect(){;}

protected:
    QRect getRectFromFourPoints( QPoint p1, QPoint p2, QPoint p3, QPoint p4);   ///< Use the min x and y values of all points to set upperLeft of rect and the max x and y values of all points to set lowerRight of rect
};


class SelectionMapper;
class MapAreaRect;
//############################## Class ZFScanMapRect ############################
/**
\brief In this TwoSpacesRect space A is the physical scan space and space B is the gray-view space.
*/
class ZFSTOOLS_API ZFScanMapRect : public TwoSpacesRect
{
public:
    ZFScanMapRect(){;}

    void setRectPhysical( QRect rect, ZFScan* scan);    ///< This also computes rectB (rect in gray view coordinates)
    void setRectFlatView( MapAreaRect rect, ZFScan* scan);        ///< This also computes rectA (rect in physical scan coordinates)

private:
    MapAreaRect getComplementaryRect(SelectionMapper* selectionMapper, MapAreaRect rect);
};

}// end namespace