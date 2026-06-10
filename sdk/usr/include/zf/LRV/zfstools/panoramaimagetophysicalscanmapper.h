#pragma once
#include "ZfsToolsAPI.h"

#include <vector>
#include <float.h>
#undef max
#undef min
#include <algorithm>

#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <QPoint>
#include <QSize>
SWITCH_WARNINGS_ON

#include "twospacesrect.h"

#include <zfs/Math/math.h>

class ZFSHeader;
class ZFSLinebyline;
class ZFScan;
class QSize;

namespace zfstools
{

class PanoramaImageToPhysicalScanMapper;

/**
\brief A TwoSpacesRect for the physical scan space and the panorama image space.

Space A is physical scan space, Space B is the bubble image space.
*/
class ZFSTOOLS_API PanoramaMapRect : public TwoSpacesRect
{
public:
    enum MapCase    // flags that are used to destinguish between the diferent map-function cases
    {
        EmptyFlags = 0x0,
        IsOnFront = 0x1,            ///< Henc < pi
        IsOnEvenPiPatch = 0x2,      ///< Lenc > pi
        IsInFirstHencInterval = 0x4 ///< If scan has two Henc-Selections (not supported anymore) and part layes within 1st selection
    };

    Q_DECLARE_FLAGS( MapCaseFlags, MapCase)
    MapCaseFlags mapCaseFlags;

    void setFlag(MapCase flg)
    {
        mapCaseFlags |= flg;
    }

    void clrFlag(MapCase flg)
    {
        mapCaseFlags &= ~flg;
    }

    void setClearFlag(MapCase flg, bool set)
    {
        if (set)
            setFlag(flg);
        else
            clrFlag(flg);
    }


    size_t lencPiPatchIndex;       ///< -1 if not set, else the Pi patch index, which is computed by dividing the covered lenc angle into parts of PI, starting with 0
    bool isUsed;    ///< If this is false, the part of the scan covered by this rect is not used for the panorama image.

public:
    PanoramaMapRect();
    PanoramaMapRect( int minScanX, int minScanY, int maxScanX, int maxScanY);
    void setRectPhysical( QRect rectInPhysicalScanSpace, PanoramaImageToPhysicalScanMapper* mapper); ///<  Returns false if the
};

Q_DECLARE_OPERATORS_FOR_FLAGS( PanoramaMapRect::MapCaseFlags)




/**
\brief A collection of values that is needed to create a PanoramaImageToPhysicalScanMapper object.

# Nomenclature #

- lenc: low-encoder, the encoder for the z-axis rotation. Low because of the lower turn rates.
- henc: high-encoder, the encoder for the rotation around the horizontal axis. High because of the higher turn rates.
- physical scan: The recorded layout of the data. This means that henc data is recorded along the x-axis and lenc data along the y-axis. There are no cuts in the physical scan.

Angles are measured in radians if not said otherwise.
*/
class ZFSTOOLS_API PanoRelevantScanParameters
{
public:
    // direct header values
    int typ;                    ///< Scan typ (Header entry 1)
    int physicalPixel;          ///< Number of columns in the physical scan
    int physicalLines;          ///< Number of rows in the physical scan
    // high encoder
    int hencStartIncrements1;   ///< Start angle of first high-encoder selection
    int hencEndIncrements1;     ///< End angle of first high-encoder selection
    int hencStartIncrements2;   ///< Start angle of second high-encoder selection
    int hencEndIncrements2;     ///< End angle of second high-encoder selection
    double hencSubsample;       ///< How many high-encoder increments between two physicalPixel
    int hencIncrementsPer2PI;   ///< Increments of the high-encoder per 360°.

    // low encoder
    int lencIncerementsPer2PI;  ///< Increments of the low-encoder per 360°.

    // helper values calculated from the header values
    double lencStartAngle;      ///< Start angle for the rotation around the -z-axis. This angle is mapped to interval [0,2 PI]
    double lencStopAngle;       ///< Stop angle for the rotation around the -z-axis. This angle is mapped to interval [0,2 PI]
    double coveredLencAngle;    ///< This can be larger than 2 PI.

    int scanLinesPerPI;         ///< Number of lines in the scan that cover 180° lenc angle.

    bool hasTwoHencIntervals;   ///< This is true if a second henc interval was scanned and hencStartIncrements2 is set.

private:
    ZFSLinebyline* lbl;
    ZFScan* scan;

public:
    PanoRelevantScanParameters( ZFScan* scan);
    PanoRelevantScanParameters( ZFSLinebyline* lbl);

    double getApproximateHencStartAngle() const;    ///< This function assumes that the first henc increment is at 0°.
    double getLencAngleBetweenScanLines() const { return PI / scanLinesPerPI;}
    double getHencAngleBetweenScanColumns() const { return 2*PI / (double)hencIncrementsPer2PI * hencSubsample ;}

private:
    const ZFSHeader* getHeader() const;
    void getScanParameter(const ZFSHeader* header);
    void computeLowEncoderAnglesWithScanContent();  ///< Get start and stop angle for the low encoder rotation.
    double lencAngleOfScanPixel(QPoint point);
    void computeDerivedScanParameters();            ///< Compute parameters that are not directly written in the header.
    void computeHasTwoHencIntervals();
    double getTheCoveredLencAngle();
    int getReflectionOfScanPixel(QPoint scanPoint);
};

/**
\brief Provides functions to map points from panorama image space to physical scan space.

The PanoramaMapper provides a function for mapping physical scan coordinates to a 360°x180° panorama image.
It also provides a function for the inverse mapping. Note that "physical" scan coordinates are not the coordinates
that are used when calling the ZFScan::Set() function and which are used in the 2d-view, but the ones that you get from ZFScan::Map().

The resulting image does not need to use the full resolution of the scan. You can provide a subsample factor
to create images with smaller resolutions. The multiple pixel in the scan will be mapped to one physicalPixel in
the image.

When mapping one must be aware of the fact, that not all points in the scan will have a corresponding point
in the panorama image and not every point in the panorama image has a corresponding point in the scan.
You should also note that the panorama image is not visually perfect, as the mapper does not take the scanner
calibration into account. There depending on the scanner there will be more or less noticeable edges at
the low-encoder positions 0° and 180°.

If you want to check if a panorama mapping of your scan is even possible, you can use the function
checkMapable().


## Angle layout of the panorama image ##

Each pixel in the panorama image can be mapped to pair of angles, a horizontal and a vertical angle.
In the panorama image the pixel with coordinates (0,0) is in the upper left corner, while the angle
pair (0,0) is in the lower left corner.

## Name conventions ##

A variable name scanX means a x coordinate in physical scan space.
A variable name imageX means a x coordinate in the panorama image space.


# Implementation details #

We can not map the full scan with a single function. Therefore we need to split it up in multiple parts
that can be mapped by a single function. When a client wants to map a point, we need to check if the
point can be mapped and if so, we need to find the appropriate function to map the point. The rect contains
the mapCaseFlags flags that we can use to quickly find the function that is needed to map this rect.

## The different map-function cases ##


When setting up the map-functions we have to distinguish between some situations.
- Is the point on the front side or back side of the scan meaning is the henc angle smaller or larger than PI.
- Is the PI-Patch number even or odd. A Pi-patch is a segment of the scan that we get when split it up at lenc-angle == n*PI borders.
Starting with patch 0, an odd patch number means that we are on the so-called "second layer".
- Is the point within the second lenc interval or not. At this point there can be two scanned henc intervals with a gap in between them.
When the point is on the second interval we need to consider the "gap" of missing data between the two intervals.


## The subsample problem ##

To generate smaller panorama images, we have a subsample factor s by which the edge length of the panorama
is divided. To avoid problems with integer division rounding, the panorama image height must be a multiple of
subsample and the panorama imgage width must be a multiple of 2*subsample. To achieve this, some lines
in the scan (at lenc = 0° and lenc = 180° borders) are neglected. We do not need to remove columns that contain data
because we can remove empty columns that lie in the scanner shadow.





Scan-Space

 .------------------.-------------------.
 |                  | evenPiPatch       |
 |                  |                   |
 |                  |                   |
 | to right (2)     | to left (4)       |
 +------------------+-------------------+  <--- hencCutCol
 | front            | front             |
 |                  | evenPiPatch       |
 |                  |                   |
 | to left (1)      | to right (3)      |
 '------------------'-------------------'
                    ^
                    '- truncatedScanLinesPerPi


Image-Space

 .------------------.-------------------.
 |                  |                   |
 |                  |                   |
 |                  |                   |
 | left             | right             |
 '------------------'-------------------'



*/
class ZFSTOOLS_API PanoramaImageToPhysicalScanMapper
{
public:
    /**
    \param subsample Use values larger then 1 to shrink the created image.
    */
    PanoramaImageToPhysicalScanMapper( ZFSLinebyline* lineByLine, unsigned int subsample = 1);
    /**
    Use this version if you created a panorama image with a PanoramaImageToPhysicalScanMapper somewhere else
    and you now need to create a mapper that fits the subsample of the created Image.
    The panoramaImageSize must come from another PanoramaImageToPhysicalScanMapper or the
    the mapper will be created for an image with subsample 1;
    */
    PanoramaImageToPhysicalScanMapper( ZFSLinebyline* lineByLine, const QSize& panoramaImageSize);
    PanoramaImageToPhysicalScanMapper( ZFScan* scan, unsigned int subsample = 1);
    PanoramaImageToPhysicalScanMapper( ZFScan* scan, const QSize& panoramaImageSize);

    // ######################   functions for mapping points #############
    inline bool mapFromScanToImage( int scanX, int scanY, int& imageX, int& imageY ) const;   ///< Returns false if the physicalPixel in the scan had no corresponding physicalPixel in the image.
    inline bool mapFromScanToImage( int& x, int& y) const;                                    ///< \overload
    inline bool mapFromScanToImage( QPoint& point) const;                                     ///< \overload

    inline bool mapFromImageToScan( int imageX, int imageY, int& scanX, int& scanY) const;    ///< Returns false if the physicalPixel in the image has no corresponding physicalPixel in the scan.
    inline bool mapFromImageToScan( int& x, int& y) const;                                    ///< \overload
    inline bool mapFromImageToScan( QPoint& point) const;                                     ///< \overload
    inline void mapClosestImagePointToScan( int imageX, int imageY, int& scanX, int& scanY) const;    ///< Shifts the given point to the closes pixel in the image that contains data and then maps it to the scan.
    // ###################################################################

    inline void putImagePointInClosestImageRect(int& imageX, int& imageY);  /**< Takes arbitrary image coordinates and searches for the nearest valid area in the panorama image. The point is then put to the closest point in that area. */

    static bool scanSupportsPanoramaImage(const ZFSHeader* header);           ///< This can be used to check whether the scan supports panorama image mapping.

    int getPanoramaImageWidth() const { return imageWidth;}        ///< Use this when you allocate your actual image.
    int getPanoramaImageHeight() const { return imageHeight;}      ///< Use this when you allocate your actual image.
    QSize getPanoramaImageSize() const;
    int getSubsampleOfImage() const { return imageSubsample;}      ///< A subsample of 2 means, that the panorama image will only have half the edge length compared to one created with subsample 1.

    int getScanWidth() const  { return scanParams.physicalPixel;}   ///< Number of pixel in physical scan.
    int getScanHeight() const { return scanParams.physicalLines;}  ///< Number of lines in physical scan.
    int getTruncatedScanPixel() const { return truncatedScanPixel;}/**< The PanoramaMapper internally shrinks the physical scan to fit the subsample. This returns the number of pixel in the shrunk scan.*/
    int getTruncatedScanLines() const { return truncatedScanLines;}///< The number of lines in the subsample conform
    ZFScan* getScan() const { return scan;}

    void imagePixelToAngle( int panoX, int panoY, double &verticalAngleRad, double &horizontalAngleRad) const;    /**< Maps image coordinates to image angles. Note that this function does not return the real/calibrated angles of the scanned point, but the angles that we get when we assume a 360°x180° image layout. */
    bool imageAnglesToScanPixel( double verticalImageAngle, double horizontalImageAngle, int& scanX, int& scanY) const; ///< Maps angles pairs from the panorama image to physical scan coordinates.
    void getApproximateScanAnglesForImagePoint( int imageX, int imageY, double& henc, double& lenc) const; ///< Takes an image point and returns its "ideal" angles in scan space.
    double getHencAngleBetweenLines() const;  ///< Returns the vertical angle that lies between two lines in the panorama image.
    double getLencAngleBetweenPixel() const;  ///< Returns the horizontal angle that lies between two columns in the panorama image.

    bool lencStartAngleIsLargerThanLencStopAngle() const { return scanParams.lencStartAngle > scanParams.lencStopAngle;}
    bool hasTwoHencIntervals() const {return scanParams.hasTwoHencIntervals;}                                                 ///< Returns true for "no top" scans.

    double getCoveredLowEncoderAngleInDegree() const; ///< lenc start angle - lenc

    bool imageCovers2PI() const;

//    double getAngleOfHighestBottomBorderOfAllImageRects();  ///< Computes the panorama image vertical angles of the lower borders of all scan parts and returns the targets of those angles.
//    double getAngleOfLowestTopBorderOfAllImageRects();      ///< Computes the panorama image vertical angles of the top borders of all scan parts and returns the smallest of those angles.

    std::vector<PanoramaMapRect> getScanParts() const { return scanParts;}     ///< Rect A is in physical scan space, rect B is in bubble image space.
    std::vector<MapAreaRect> getMapAreaRectsOfPhysicalScan() const;           ///< returns the MapAreaRects in physical scan space.

    MapAreaRect getPanoramaImageDataBoundingBox() const;      ///< It is guaranteed that there is no data in the panorama image outside of the returned rect (test outsideness by calling !MapAreaRect::pointIsInRect()).

    QPointF getAngleOffsetToPanoramaBoundingBox() const;      ///< The angle offset in rad of the upper left corner of the image data bounding box.
    QPointF getAngleOffsetToPanoramaRect(QRect rect) const;  ///< The angle offset in rad of the upper left corner of the rect.
    bool isFullPanorama() const;                              ///< This returns true if the data area in the panorama image covers the full 360° lenc interval.

    inline PanoramaMapRect::MapCaseFlags getMapFlagsForScanPoint( int scanX, int scanY, bool& mapable) const;     ///< Checks in which scan part the point lies and if this part is used for mapping.
    inline PanoramaMapRect::MapCaseFlags getMapFlagsForImagePoint( int imageX, int imageY, bool& mapable) const;  ///< Does the same as getMapFlagsForScanPoint() but in image space.

    int getCutCollumn() const { return hencCutCol;}            ///< The scan is vertically cut in half left of the returned column.

    /// print out some debug informations
    std::string dump() const;

protected:
    // map function for each scan part case.
    inline void mapRectPointFromScanToImage( PanoramaMapRect::MapCaseFlags rectFlags, int scanX, int scanY, int& imageX, int& imageY) const;
    inline void mapRectPointFromImageToScan( PanoramaMapRect::MapCaseFlags rectFlags, int imageX, int imageY, int& scanX, int& scanY) const;

private:
    void init(ZFScan* scan, ZFSLinebyline* lbl, int subsample);    ///< initialize member
    int computeSubsampleWithPanoImageSize(const QSize& panoImageSize);

    void computeMappingParameterAndContinousDataRectangles(const ZFSHeader* header);

    void computeMappingParameter();                         ///< compute the mapping parameter that are derived from the scan parameter.
    void computeHencCutCol();                               ///< Compute the column in the physical scan at which the scan is cut in half for unfolding.
    bool isFullFrontBackScan() const;                             ///< This returns true if we have a normal front back scan with 180° low encoder scanning.
    enum Angle {HENC_ANGLE, LENC_ANGLE};
    double angleOfScanPixel( int scanX, int scanY, Angle angle) const;  ///< Merges a repeatedly used combination of three functions into one.


    int limitEndPixelToValidArea(int endX) const;
    int limitStartPixelToValidArea(int startX) const;

    void createScanDataRectangles();        /**< This uses the information from PanoramaMapper::scanParams to compute PanoramaMapper::scanParts. One scanPart is an rectangle in the scandata that can be mapped by a single function. */
    void createScanRectsWithHeaderInformation();
    void devideScandataRectIntoMappablePartsAndAddThemToScanParts( PanoramaMapRect rect);
    int getScanXFromHencIncrement( int hencIncrement) const;
    void devideRectLeftOfColumnAndPutPartsInScanRects( PanoramaMapRect rect, int column);
    void computeBoundingRectsInImageSpace();
    void computBoundingRectInImageSpace(PanoramaMapRect& rect) const;

    void chooseUsedScanDataRectangles();    ///< Decide which parts of the scan will be used for the panorama image.
    bool chooseUsedScanDataRectanglesForSecondLayerCase();

    bool rectIsMappedToLeftImageSide(PanoramaMapRect::MapCaseFlags flgs) const;
    size_t getIndexAndHencIntervalOfLargestArea(const std::vector<double>& firstHencIntervalAreas, const std::vector<double>& secondHencIntervalAreas, bool& useFirstInterval) const;

    inline PanoramaMapRect::MapCaseFlags setImagePointToClosestImageRect( int& imageX, int& imageY) const;    ///< returns the flags of the rect in which the shifted point lies
#pragma warning( suppress : 4251) // deactivate warning for next line
    inline double computeSquareDistanceToRectInImageSpaceAndShiftPointOnThatRect( int imageX, int imageY, PanoramaMapRect rect, int& shiftedImageX, int& shiftedImageY) const;

    void computeShiftableSelectionScanAngleCorrectionOffset();
    QPoint computeShiftOffset() const;
    void applySelectionScanOffsetToPanoramaImageDataRectangles();

    void getApproximateCalibratedLencStart();  ///< Gets the lenc value of a pixel in the first scan row.

    void angleToImagePixel( double verticalAngle, double horizontalAngle, int& imageX, int& imageY) const;
    inline int imageXDistance( int imageX1, int imageX2 ) const;     /**< Computes the distance between two columns in the panorama image considering the periodicity of the x-coordinate.*/
    void useFirstTwoPiecesOnTheFront();

    int getNumberOfPiPatches() const;
    int getNumberOfPiPatchesFromScanParts() const;

protected:
    ZFScan* scan;

    #pragma warning( suppress : 4251) // deactivate warning for next line
    PanoRelevantScanParameters scanParams;

    QPoint shiftableSelectionScanAngleCorrectionOffset;  ///< This is an offset in the image that is used when we have a "shiftable" selection scan, to better match the angles of the panorama image with the angles of the scan.
    double approximateCalibratedLencStart;    /**< The lenc of a pixel in the first row in the scan from the scan data. If no scan data is available it is the lenc start value from the header. The value is used when mapping panorama angles to scan angles*/

private:
    ZFSLinebyline* lbl;                     ///< The ZFSLinebyline is used to compute the correction for the PI-Border artifact problem.

    SWITCH_WARNINGS_OFF
    std::vector<PanoramaMapRect> scanParts;                 ///<    The scan rectangles. For each we have a simple mapping function. Rect A physical, Rect B bubble image
    std::vector<int> indicesOfLinesAfterMissingLineGaps;    ///< The indices of the first line missing line intervals.
    SWITCH_WARNINGS_ON

    int imageSubsample;                     ///< Factor that is used to shrink the size of the panorama image.

    int hencEqualsPICol;                    /**< (imaginary) x-value in the physical scan where the henc is at 180°. Imaginary because it only exists in a full scan. */
    int hencCutCol;                         /**< (imaginary) x-value in the physical scan at which the scan is cut in half. Imaginary because it only exists in a full scan. The border lies left of this column*/
    int offsetFirstHencIntervalToVertical;  /**< Distance between the hencEqualsPICol and the first henc interval (imaginary pixel). Value is only relevant if we have two henc intervals */
    int offsetSecondHencIntervalToVertical; /**< Distance between the hencEqualsPICol and the second henc interval (imaginary pixel). Value is only relevant if we have two henc intervals */
    int truncatedScanlinesPerPi;            ///< This is scanlinesPerpi - scanlinesPerpi % imageSubsample
    int truncatedScanLines;                 ///< truncatedScanLines % imageSubsample == 0
    int truncatedScanPixel;                 ///< truncatedScanPixel % imageSubsample == 0

    int imageHeight;        ///< Height of the panorama image.
    int imageWidth;         ///< Width of the panorama image.
    int halfImageWidth;     ///< imageWidth/2, saved because it is used in map functions.
};

/**
This function maps points in (physical) scan space to points in the panorama image.
The point (0|0) is in the upper left corner of the scan. The positive x-axis goes to the right,
the positive y-axis goes down.

\param scanX x coordinate of a pixel in the physical scan space.
\param scanY y coordinate of a pixel in the physical scan space.
\param imageX Computed x coordinate of the scan pixel in the panorama image.
\param imageY Computed y coordinate of the scan pixel in the panorama image.
\return Returns false when the scan pixel can not be mapped to a panorama image pixel. This can happen for "second layer" scans.
*/
bool PanoramaImageToPhysicalScanMapper::mapFromScanToImage( int scanX, int scanY, int& imageX, int& imageY ) const
{
    bool pointCanBeMapped = false;
    PanoramaMapRect::MapCaseFlags flags = getMapFlagsForScanPoint( scanX, scanY, pointCanBeMapped);
    if( !pointCanBeMapped )
    {
        imageX = 0;
        imageY = 0;
        return false;
    }

    mapRectPointFromScanToImage( flags, scanX, scanY, imageX, imageY);
    return true;
}

bool PanoramaImageToPhysicalScanMapper::mapFromScanToImage( int& x, int& y) const
{
    int scanX = x;
    int scanY = y;
    return mapFromScanToImage( scanX, scanY, x, y);
}

bool PanoramaImageToPhysicalScanMapper::mapFromScanToImage(QPoint& point) const
{
    return mapFromScanToImage( point.x(), point.y(), point.rx(), point.ry());
}

PanoramaMapRect::MapCaseFlags PanoramaImageToPhysicalScanMapper::getMapFlagsForScanPoint( int scanX, int scanY, bool& mapable) const
{
    // find the scan part in which the point lies.
    for (size_t i=0; i<scanParts.size(); i++)
    {
        if (scanParts[i].rectA.left() <= scanX &&
            scanX < scanParts[i].rectA.right() &&
            scanParts[i].rectA.top() <= scanY &&
            scanY < scanParts[i].rectA.bottom())
        {
            mapable = scanParts[i].isUsed;
            return scanParts[i].mapCaseFlags;
        }
    }
    mapable = false;
    return PanoramaMapRect::EmptyFlags;
}

/**
Use this function to map points without checking if they really belong to a certain MapAreaRect.
*/
void PanoramaImageToPhysicalScanMapper::mapRectPointFromScanToImage( PanoramaMapRect::MapCaseFlags rectFlags, int scanX, int scanY, int& imageX, int& imageY) const
{
    // typ 1000 ist _rgb.zfs scan, is a re-sampled scan type with integrated color and equally spaced angles.
    if( scanParams.typ == 1000 )
    {
        imageX = scanX;
        imageY = scanY;
        return;
    }

    // shift point to imaginary full scan space if it is on second henc interval
    if( !(rectFlags & PanoramaMapRect::IsInFirstHencInterval) )
    {
        scanX += offsetFirstHencIntervalToVertical + offsetSecondHencIntervalToVertical;    // add gap between henc intervals
        rectFlags |= PanoramaMapRect::IsInFirstHencInterval;    // now we can act as if the point is in first interval
    }

    // choose the appropriate map function
    // The asymmetric int rounding has the advantage, that to subsample scan points belong to each image point.
    // With normal int truncation we will get more pixel for the image coord == 0 pixels.
    switch( rectFlags)
    {
        case 4: // IsInFirstHencInterval = true, IsOnEvenPiPatch = false, IsOnFront = false
            imageY = zfs::math::assymToInt( (double)(scanX - hencCutCol) / imageSubsample);
            imageX = zfs::math::assymToInt( (double)(scanY - truncatedScanlinesPerPi) / imageSubsample);
            break;
        case 6: // IsInFirstHencInterval = true, IsOnEvenPiPatch = true, IsOnFront = false
            imageY = zfs::math::assymToInt( (double)(scanX - hencCutCol) / imageSubsample);
            imageX = zfs::math::assymToInt( (double)(truncatedScanlinesPerPi + scanY) / imageSubsample);
            break;
        case 5: // IsInFirstHencInterval = true, IsOnEvenPiPatch = false, IsOnFront = true
        case 7: // IsInFirstHencInterval = true, IsOnEvenPiPatch = true, IsOnFront = true
            imageY = zfs::math::assymToInt( (double)(hencCutCol - 1 - scanX) / imageSubsample);
            imageX = zfs::math::assymToInt( (double)scanY / imageSubsample);
            break;
        default:    // should not be reached because we set the the flag PanoramaMapRect::IsInFirstHencInterval to true in before the switch.
            imageX = 0;
            imageY = 0;
    }

    // apply angle matching offset for selection scans
    imageX += shiftableSelectionScanAngleCorrectionOffset.x();
    imageY += shiftableSelectionScanAngleCorrectionOffset.y();
}

/**
The point (0|0) is in the upper left corner of the panorama image.
The positive x-axis goes to the right, the positive y-axis goes down.

\param imageX x coordinate of a panorama image pixel.
\param imageY y coordinate of a panorama image pixel.
\param scanX x coordinate of the panorama image pixel in the physical scan.
\param scanY y coordinate of the panorama image pixel in the physical scan.
\return Returns false if the given panorama image point has no corresponding point in the scan. This can happen for selection scans.
*/
bool PanoramaImageToPhysicalScanMapper::mapFromImageToScan( int imageX, int imageY, int& scanX, int& scanY) const
{
    bool pointCanBeMapped = false;
    PanoramaMapRect::MapCaseFlags flags = getMapFlagsForImagePoint( imageX, imageY, pointCanBeMapped);
    if( !pointCanBeMapped )
    {
        scanX = 0;
        scanY = 0;
        return false;
    }

    mapRectPointFromImageToScan( flags, imageX, imageY, scanX, scanY);
    return true;
}

PanoramaMapRect::MapCaseFlags PanoramaImageToPhysicalScanMapper::getMapFlagsForImagePoint( int imageX, int imageY, bool& mapable) const
{
    // find the scan part in which the point lies.
    for( size_t i=0; i<scanParts.size(); i++)
    {
        if( scanParts[i].rectB.left() <= imageX && imageX < scanParts[i].rectB.right() && scanParts[i].rectB.top() <= imageY && imageY < scanParts[i].rectB.bottom() && scanParts[i].isUsed )
        {
            mapable = true;
            return scanParts[i].mapCaseFlags;
        }
    }
    mapable = false;
    return PanoramaMapRect::EmptyFlags;
}

void PanoramaImageToPhysicalScanMapper::mapRectPointFromImageToScan( PanoramaMapRect::MapCaseFlags rectFlags, int imageX, int imageY, int& scanX, int& scanY) const
{
    if( scanParams.typ == 1000 )
    {
        scanX = imageX;
        scanY = imageY;
        return;
    }

    // remove angle matching offset for selection scans
    imageX -= shiftableSelectionScanAngleCorrectionOffset.x();
    imageY -= shiftableSelectionScanAngleCorrectionOffset.y();

    switch( rectFlags)
    {
    case 0: // IsInFirstHencInterval = false, IsOnEvenPiPatch = false, IsOnFront = false
    case 4: // IsInFirstHencInterval = true, IsOnEvenPiPatch = false, IsOnFront = false
        scanY = imageX * imageSubsample + truncatedScanlinesPerPi;
        scanX = imageY * imageSubsample + hencCutCol;
        break;
    case 2: // IsInFirstHencInterval = false, IsOnEvenPiPatch = true, IsOnFront = false
    case 6: // IsInFirstHencInterval = true, IsOnEvenPiPatch = true, IsOnFront = false
        scanY = imageX  * imageSubsample - truncatedScanlinesPerPi;
        scanX = imageY * imageSubsample + hencCutCol;
        break;
    case 1: // IsInFirstHencInterval = false, IsOnEvenPiPatch = false, IsOnFront = true
    case 3: // IsInFirstHencInterval = false, IsOnEvenPiPatch = true, IsOnFront = true
    case 5: // IsInFirstHencInterval = true, IsOnEvenPiPatch = false, IsOnFront = true
    case 7: // IsInFirstHencInterval = true, IsOnEvenPiPatch = true, IsOnFront = true
        scanY = imageX * imageSubsample;
        scanX = hencCutCol - imageY * imageSubsample - imageSubsample;
        break;
    }

    // now handle the second henc interval case
    if( !(rectFlags & PanoramaMapRect::IsInFirstHencInterval) )
    {
        scanX -= (offsetFirstHencIntervalToVertical + offsetSecondHencIntervalToVertical);    // add gap beteween henc intervals
    }
}

bool PanoramaImageToPhysicalScanMapper::mapFromImageToScan( int& x, int& y) const
{
    int imageX = x;
    int imageY = y;
    return mapFromImageToScan( imageX, imageY, x, y);
}

bool PanoramaImageToPhysicalScanMapper::mapFromImageToScan( QPoint& point) const
{
    return mapFromImageToScan( point.x(), point.y(), point.rx(), point.ry());
}

/**
This variants can take coordinates of image points that do not contain scan data or are not even on the image.

The point (0|0) is in the upper left corner of the panorama image.

\param imageX x coordinate of a panorama image pixel.
\param imageY y coordinate of a panorama image pixel.
\param scanX x coordinate of the panorama image pixel in the physical scan.
\param scanY y coordinate of the panorama image pixel in the physical scan.
*/
void PanoramaImageToPhysicalScanMapper::mapClosestImagePointToScan( int imageX, int imageY, int& scanX, int& scanY) const
{
    PanoramaMapRect::MapCaseFlags flags = setImagePointToClosestImageRect( imageX, imageY);

    mapRectPointFromImageToScan( flags, scanX, scanY, imageX, imageY);
}

/**
If the point is already in a valid area, nothing will happen.
*/
void PanoramaImageToPhysicalScanMapper::putImagePointInClosestImageRect( int& imageX, int& imageY)
{
    setImagePointToClosestImageRect( imageX, imageY);
}

PanoramaMapRect::MapCaseFlags PanoramaImageToPhysicalScanMapper::setImagePointToClosestImageRect( int& imageX, int& imageY) const
{
    double shortestDistance = DBL_MAX;
    int orgX = imageX;
    int orgY = imageY;
    int shiftedX = imageX;
    int shiftedY = imageY;
    PanoramaMapRect::MapCaseFlags closestRectFlags;
    for( size_t i=0; i<scanParts.size(); i++)
    {
        if( scanParts[i].isUsed )
        {
            double distance = computeSquareDistanceToRectInImageSpaceAndShiftPointOnThatRect( orgX, orgY, scanParts[i], shiftedX, shiftedY);
            if( distance < shortestDistance)
            {
                shortestDistance = distance;
                imageX = shiftedX;
                imageY = shiftedY;
                closestRectFlags = scanParts[i].mapCaseFlags;
            }
        }
    }
    return closestRectFlags;
}

double PanoramaImageToPhysicalScanMapper::computeSquareDistanceToRectInImageSpaceAndShiftPointOnThatRect( int imageX, int imageY, PanoramaMapRect rect, int& shiftedImageX, int& shiftedImageY) const
#pragma warning( suppress : 4273) // deactivate warning for next line
{
    bool isOnSameWidth = rect.rectB.left() <= imageX && imageX < rect.rectB.right();
    bool closerToLeftX = imageXDistance( rect.rectB.left(), imageX) <= imageXDistance( rect.rectB.right(), imageX);
    bool isLeftOfRect = closerToLeftX && !isOnSameWidth;   // this takes account for the periodicity of the image x coordinate
    bool isRightOfRect = !closerToLeftX && !isOnSameWidth;   // this takes account for the periodicity of the image x coordinate
    bool isAboveRect = imageY < rect.rectB.top();
    bool isBelowRect = imageY >= rect.rectB.bottom();
    bool isOnSameHeight = !isAboveRect && !isBelowRect;

    int maxXInRect = rect.rectB.right()-1;
    int maxYInRect = rect.rectB.bottom()-1;

    shiftedImageX = imageX;
    shiftedImageY = imageY;

    if( isOnSameHeight && isOnSameWidth ) // point is in rect, for random points this should be at the end, but people will click on the data most of the times.
    {
        return 0;
    }
    else if( isAboveRect && isLeftOfRect )
    {
        shiftedImageX = rect.rectB.left();
        shiftedImageY = rect.rectB.top();
        return sqare_line_length_2D( imageX, imageY, rect.rectB.left(),  rect.rectB.top());
    }
    else if( isBelowRect && isLeftOfRect)
    {
        shiftedImageX = rect.rectB.left();
        shiftedImageY = maxYInRect;
        return sqare_line_length_2D( imageX, imageY, rect.rectB.left(),  maxYInRect);
    }
    else if( isAboveRect && isRightOfRect)
    {
        shiftedImageX = maxXInRect;
        shiftedImageY = rect.rectB.top();
        return sqare_line_length_2D( imageX, imageY, maxXInRect,  rect.rectB.top());
    }
    else if( isBelowRect && isRightOfRect)
    {
        shiftedImageX = maxXInRect;
        shiftedImageY = maxYInRect;
        return sqare_line_length_2D( imageX, imageY, maxXInRect,  maxYInRect);
    }
    else if( isOnSameHeight && isLeftOfRect)
    {
        shiftedImageX = rect.rectB.left();
        return (imageX-rect.rectB.left())*(imageX-rect.rectB.left());
    }
    else if( isOnSameHeight && isRightOfRect)
    {
        shiftedImageX = maxXInRect;
        return (imageX-maxXInRect)*(imageX-maxXInRect);
    }
    else if( isOnSameWidth && isAboveRect)
    {
        shiftedImageY = rect.rectB.top();
        return (imageY-rect.rectB.top())*(imageY-rect.rectB.top());
    }
    else //( isOnSameWidth && isBelowRect)
    {
        shiftedImageY = maxYInRect;
        return (imageY-maxYInRect)*(imageY-maxYInRect);
    }
}

//---------------------------------------------------------------------------------------------
/**
\param  x-coordinate of vertical line 1 in bubble-image space. If the value is outside the allowed values, it will be mapped back into the
\param  x-coordinate of vertical line 2 in bubble-image space.
\return Distance between point 1 and point 2. This value is always positive.
*/
int PanoramaImageToPhysicalScanMapper::imageXDistance( int imageX1, int imageX2 ) const
{
    int dx = 0;

    if (imageWidth > 0) {
    // periodic back-map
        while (imageX1 < 0)
            imageX1 += imageWidth;
        while (imageX1 > imageWidth - 1)
            imageX1 -= imageWidth;
        while (imageX2 < 0)
            imageX2 += imageWidth;
        while (imageX2 > imageWidth - 1)
            imageX2 -= imageWidth;

        dx = abs(imageX1 - imageX2);
        if (dx > imageWidth / 2) {
            dx = std::min(imageX1, imageX2);
            dx += imageWidth - std::max(imageX1, imageX2);
        }
    }
    return dx;
}

} // end namespace
