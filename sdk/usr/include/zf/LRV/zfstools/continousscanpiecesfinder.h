#pragma once
#include "ZfsToolsAPI.h"

#include <vector>

#include <Common/def.h>
SWITCH_WARNINGS_OFF
#include <QRect>
#include <QPoint>
SWITCH_WARNINGS_ON

#include "twospacesrect.h"

class ZFScan;
class ZFSLinebyline;

namespace zfstools
{

/**
\brief The ContinousScanPiecesFinder can identify the rectangles in a ZFScan which can be mapped from and to physical scan-space with one simple linear function.

When looking at a scan in the flat-view, one can identify a couple of rectangular areas that are divided by lines of discontinuity.
The SmoothScanPiecesFinder searches for these lines and returns a couple of rectangles that cover the scan and do not
contain such lines of discontinuity. The discontinuities are then on the lines that separate the rectangles.

To keep thing simple, for now we do not handle the general case, but assume that the discontinuities in the flat view
space are a vertical line at the middle of the scan and another discontinuity line in the right half
of the scan in flat-view space.
*/
class ZFSTOOLS_API ContinousScanPiecesFinder
{
private:
    ZFScan* scan;
    ZFSLinebyline* lineByLine;

    SWITCH_WARNINGS_OFF
    std::vector<ZFScanMapRect> pieces;
    SWITCH_WARNINGS_ON

    int xOfVerticalDiscontinuity;   ///< The discontinuity where the front and back half are put together.
    int yOfHorizontalDiscontinuity; ///< The discontinuity at the bottom of the right scan side.

public:
    ContinousScanPiecesFinder(ZFScan* scan);
    
    std::vector<ZFScanMapRect> findContinousScanPieces(); ///< Cut the scan apart at the discontinuities and return the single pieces.
    std::vector<MapAreaRect> findContinousPiecesInFlatViewSpace();
    std::vector<MapAreaRect> findContinousPiecesInPhysicalScanSpace();

private:
    QPoint goThroughGrayViewAndFindDiscontinuitiy( QPoint startPoint, bool goRight);
    QPoint mapPointPhysical( QPoint point);
    bool pointsAreNeighbors( QPoint p1, QPoint p2);
    std::vector<ZFScanMapRect> getTwoSpacesRectsWithFlatViewRects( const std::vector<MapAreaRect>& flatViewRects);
};

} // end namespace