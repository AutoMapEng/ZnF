#ifndef POLYLINE_H
#define POLYLINE_H

#include <vector>

#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <QPoint>
SWITCH_WARNINGS_ON

#include "ZfsToolsAPI.h"



class ZF2DSelection;
class ZF2DSelectionRect;
class ZF2DSelectionPoly;
class QRect;
class QPointF;
class QLine;
class QPolygon;

SWITCH_WARNINGS_OFF
typedef std::vector<QPoint> QPoints;
SWITCH_WARNINGS_ON

namespace zfstools
{
    //############################# Class PolyLine #####################
    /**
    \brief A vector of points. We imagine the points to be connected by lines.
    */
#pragma warning(push)
#pragma warning(disable: 4251 )
    class ZFSTOOLS_API PolyLine : public QPoints
    {
    public:
        PolyLine(){}
        PolyLine( ZF2DSelection* selection);
        /** 
        \brief Creates a closed PolyLine with points on the corners of the rect.
        
        The upper left point of the rect exists twice in the polyline.
        */
        PolyLine( const QRect& rect);

        void addPoints( PolyLine points);                   ///< Add the points of the other line to this line.
        void addPointsInInvertedOrder( PolyLine points);    ///< Add the points of the other line to this starting at the end of the other line.
        void close();                                       ///< Add the fist point at the end.

        bool isSelfIntersecting() const;                    ///< Returns true if one or more line segments intersect each other.
        bool pointLiesInPolygonInterior( QPoint point) const;           ///< Returns false if the the point lies on one of the lines or outside of the polygon.
        bool pointLiesInPolygonInterior( int x, int y) const;
        bool pointLiesInPolygonInteriorOrOnBorder( QPoint point) const; ///< Returns true if a point lies in the interior of the polygon or on one of its lines.
        int pointLiesWithinPolygonJordan( QPoint point) const;          ///< Returns 1 if inside, -1 if outside, 0 if on line, Method after Jordan http://de.wikipedia.org/wiki/Punkt-in-Polygon-Test_nach_Jordan
        bool contains( const PolyLine& polyline) const;                 ///< Returns true if all points in polyline lie within this.  
        bool isRect() const;                                            ///< Returns true if the polylines form a rect. The first point can be the same as the last.
        bool hasPoint( QPoint point) const;                             ///< Returns true if one of the polyline points is the same as point.

        QPointF computeCenterOfMass() const;
        double computeArea() const;
        QRect boundingRect() const;

        void addPointsToZF2DSelectionPoly( ZF2DSelectionPoly* selection) const;   ///< Adds all the points in this to selection.

    private:
        void copyPointsFromRectSelection( ZF2DSelectionRect* selection);
        void copyPointsFromPolySelection( ZF2DSelectionPoly* selection);
        int crossProductTest( QPoint testPoint, QPoint linePoint1, QPoint linePoint2) const;
        bool isSelfIntersectingOrHasReapeatedPoints(const PolyLine& poly) const;
    };


    //############################# Class PolyLines #####################
    /// A std::vector of PolyLine objects.
    class ZFSTOOLS_API PolyLines : public std::vector<PolyLine>
    {
    public:
        SWITCH_WARNINGS_OFF
        using std::vector<PolyLine>::push_back;
        SWITCH_WARNINGS_ON
        void push_back( PolyLines lines);           ///< add multiple polylines to this.
    };
#pragma warning(pop)

}

#endif // POLYLINE_H