#ifndef ZFPOLYGON_H
#define ZFPOLYGON_H

#include "../zfs/trmatrix.h"

#include "obj.h"
#include "plane.h"

class QPixmap;

namespace zfprj
{

/**
\brief The ZfPolygon consists of a set of 2D-points and a definition of a plane in 3D-space.

The 2d-points lie on the plane, which also defines the local coordinate system of the polygon.
The plane on which the points lie can either be defined manually with \ca ZFPolygon::setPlane()
or the plane will be defined automatically by the first three added points.

The ZfPolygon is used to create floor plans and to calculate areas.


Implementation details:
To make ist possible to add points to the polygon without knowing its plane, added
3d-points are stored in their original value and are not projected to the plane.
You can get projected points with get3DPointsOnPlane().

Refactoring:
The poly-plane mode of ZFPlane should be replaced 
*/
class ZFSPRJ_API ZFPolygon : public ZFPrjObj
{
    DEFINE_ZFBASE(ZFPrjObj)

public:
    static const double fillTransparency;   ///< The transparency of the interior of the polygon when its drawn.
    static const int lineWidthFactor3d;     ///< The the user set linewidth is multiplied by this factor when displaying the polygon in the 3d view.

    ZFTRMatrix localToGlobalSytemMatrix;    ///< Matrix that defines the local coordinate system of the polygon.
private:
    /** Defines how the polygon gets its local coordinate system i.e. the plane on which the points lie. */
    enum planeDefinitionMode { 
        SET_MANUALLY,                   /**< The local system was set by the user */
        SET_WITH_FIRST_THREE_POINTS     /**< The local system was computed by using the plane that is defined by the first three polygon points. */
    };

    planeDefinitionMode planeMode;
    ZFTRMatrix globalToLocalSystemMatrix;   ///< The inverse matrix of ZFPolygon::localToGlobalSytemMatrix.
    plane_points_t polyPoints;              ///< The points that define the polygon. Note that the 3d coordinates are the original coordinates from the add3DPoint() function.

   
public:
    ZFPolygon( std::wstring name);
    ZFPolygon( ZFPrjObjNode* parent,   tinyxml2::XMLNode* node); ///< Create a plane by reading its data from an  tinyxml2::XMLNode.

    void tansformPolygonCoordinates( ZFTRMatrix oldToNewSystemMatrix);  /**< This changes the coordinate system of the polygon. The matrices and 3d points will be changed when calling this. */

    /**
    \brief Add a point to the polygon given in global 3d-coordinates. 
    \return Returns false if the new point causes a line intersection
    */
    bool add3DPoint( double x, double y, double z); 
    bool add3DPoint( const ZFVec3d& point); 
    bool add2DPoint( double u, double v);           ///< Add a point to the polygon given in local 2d-coordinates.
    void changeLastPoint( double x, double y, double z);    ///< Changes the 3d coordinates of the last point in polyPoints.
    void deleteLastPointAndUpdateMember();          ///< Deletes the last point in ZFPolygon::polyPoints and recalculates the uv coordinates and plane matrix.
    void clearPoints();                             ///< Deletes all points

    size_t getNumberOfPoints() const;                     ///< Returns the number of points in the polygon.
    ZFPlanePolyPnt getPoint( size_t i) const;             ///< Returns the ith point in polyPoints.
    bool empty() const { return polyPoints.empty();}       ///< Returns true if there are no points in the polygon.
    plane_points_t getPolyPoints() const { return polyPoints;} /**< The points of the polygon. Note that the coordinates stored in x,y,z are the original points in scan coordinates and do not lie on the plane. */
    bool closingLineIntersectsOtherLine() const;          ///< Checks if the line from the last point to the first points intersects any other line in the polygon.
    /**
    \return Returns true if the line between the point with index size-1 and the one with index
    size-2 crosses another line in the polygon.
    */
    bool lastLineCrossesPreviousLines() const;

    void setPlane( double x, double y, double z, double nX, double nY, double nZ);  ///< Set the plane in which the polygon lives with a point and a normal vector.
    ZFTRMatrix getPlaneMatrix() const { return localToGlobalSytemMatrix; }            ///< Returns the matrix that transforms points from local coordinates to global coordinate.

    double getPolygonArea() const;                        ///< Returns the area of the polygon in square-meter.
    double getPolygonCircumference() const;               ///< Returns the circumference of the polygon in meter.

    plane_points_t get3DPointsOnPlane() const;            ///< The 3D coordinated in the returned points will not be the coordinates of the originally added points, but the projection of those points on the polygon plane.
    void projectPointOnPlane( ZFPlanePolyPnt& point) const;   ///< Will project the point to the polygon plane.
    ZFPlanePolyPnt calculateCenterOfMass() const;         ///< Calculates the center of mass for the ZFPolyogn::polyPoints.

    std::string Xml()  const override { return "Polygon"; }  ///< Xml section where to store

    bool readValues(  tinyxml2::XMLNode* node);     ///< Store the data of the polygon to node.
    bool writeValues(   tinyxml2::XMLNode* node);   ///< Write the data in node to the data members of the ZFPolygon.            

    virtual const QPixmap* GetPixmap();     /// 
    static void clearPixmap();

    bool drawAreaText;

private:
    void init();    ///< Initializes all member variables.

    void useTheFirstPointsToDefineThePolygonPlane();        ///< Computes the ZFPolygon::localToGlobalSytemMatrix using the first points in ZFPolygon::polyPoints.
    void recalculateUVCoordinates();                        ///< Recalculates all u-v coordiantes of the ZFPolygon::polyPoints with the current local coordinate system.
    void calculatePlaneSystemWithOnePoint();                ///< Computes a local coordinate system using the first point in ZFPolygon::polyPoints.
    void calculatePlaneSystemWithTwoPoints( size_t i, size_t j);               ///< Computes a local coordinate system using the first two points in ZFPolygon::polyPoints.
    void calculatePlaneSystemWithThreePoints();             ///< Computes a local coordinate system using the first three points in ZFPolygon::polyPoints.
    void copySystemToTransformationMatrix( const ZFVec3d& uX, const ZFVec3d& uY, const ZFVec3d& uZ, const ZFVec3d* translate = NULL); ///< Copies the basis defined by the first 4 vectors to the ZFPolygon::localToGlobalSytemMatrix and ZFPolygon::globalToLocalSystemMatrix.
    void setCenterOfLocalSystem( double x, double y, double z);                     ///< Sets the center of the local coordinate system.
    void calculateUVCoordinatesOfPolyPoint( ZFPlanePolyPnt& point) const;                 ///< Uses the current ZFPolygon::globalToLocalSystemMatrix to calculate the u-v coordinates of a point.

    void rotateLocalSystemForAGoodOrthophotoDefault();                      ///< This will swap and mirror the axes of the local system to make x lie close to the global horizontal and y close to the global z axis.
    void createPlaneSystemWithPointAnNormal(double x, double y, double z, double nX, double nY, double nZ); ///< This will set the local system matrices using the given information.
    void putLocalCoordinateCenterToCenterOfMass();
    bool lineIntersectsOtherLine( const ZFPlanePolyPnt& p1, const ZFPlanePolyPnt& p2) const;  ///< Checks if the line defined by p1(u,v) and p2(u,v) intersects any other line of the polygon.
    void recalculateMatrixAndUVCoordinatesAfterChangeInPolypoints();    ///< Uses the 3D-data in ZFPolygon::polyPoints to calculate ZFPolygon::localToGlobalSytemMatrix and the uv-Coordinates of the points.
};

/**
\brief The project node that holds all the polygons.

It is responsible for storing and loading the planes from the project file.

TODO
*/
class ZFSPRJ_API ZFPolygons : public ZFPrjObjNode
{
    DEFINE_ZFBASE(ZFPrjObjNode)

private:
    static int nameCounter;                 ///< This is used for generating the default name of a plane.
    static std::wstring defaultNamePrefix;  ///< This is used for generating the default name of a plane.

public:
    ZFPolygons( ZFProject* project );

    void addPolygon( ZFPolygon* polygon);               ///< The ownership of the polygon goes to ZFPolygons.
    ZFPolygon* findPolygon( const std::wstring& name);  ///< Ownership stays with ZFPolygons.
    ZFPolygon* getLastPolygon();

    virtual bool Load();                                ///< Load all polygons from project file.
    virtual bool Store( double versNr );                               ///< Store all polygons in project file.

    QString Typ() const override { return QObject::tr("Polygons"); }   ///< Typename
    std::string Xml() const override { return "Polygons"; }    ///< Xml section where to store.

    static void setDefaultNamePrefix( std::wstring prefix){ defaultNamePrefix=prefix;}
    static std::wstring getDefaultName(); 
};

}   // namespace zfprj

#endif //ZFPOLYGON_H