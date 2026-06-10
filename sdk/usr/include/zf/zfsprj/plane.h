#pragma once

/**
    classes holding planes stored in project
*/

#include "ZfsPrjBasic.h"
#include "obj.h"
#include "../zfs/zfvector.h"
#include "../zfs/trmatrix.h"

#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <QString>
SWITCH_WARNINGS_ON

#include <list>
#include <ostream>

#define PROJECT_Z_PLANE L"Auto Z Plane"  //Name der Z Ebene ueber das gesammte Projekt

class ZFProject;
class QPointF;

namespace tinyxml2 {
    class XMLNode;
}

namespace zfprj
{
    //=====================================================================================================
    /**
    \brief A corner point in a ZFPlane object.
    */
    class ZFSPRJ_API ZFPlanePolyPnt
    {
    public:
        double x, y, z;   ///< Global 3d-coordinates.
        double u, v;     ///< 2d-coordinates of the point in plane space.
        ZFPlanePolyPnt();
        ZFPlanePolyPnt(double xArg, double yArg, double zArg, double uArg = 0, double vArg = 0);
        ZFPlanePolyPnt(const ZFVec3d& vec, const QPointF& scanPoint);

        const ZFVec3d get3D() const;
    };

    ZFSPRJ_API bool operator==(const ZFPlanePolyPnt& lhs, const ZFPlanePolyPnt& rhs);
    ZFSPRJ_API std::ostream& operator<<(std::ostream& lhs, const ZFPlanePolyPnt& rhs);

    typedef std::vector<ZFPlanePolyPnt> plane_points_t;

    std::vector<ZFVec3d> ZFSPRJ_API planePointsToZFVecs(const plane_points_t& planePoints);

    /**
    \brief This class handles rectangular planes (PLANE_HESSE_WH) and polygons (PLANE_POLY) that consist of 3 or 4 points.

    When a rectangle plane is constructed, the points on its corners are automatically computed.

    When a polygon is constructed it will also create a rectangle. Be aware of the
    fact that this rectangle will not exactly match the polygon as the points of the polygon will
    not necessarily lie in one plane. If they do, the rectangle will be a bounding rectangle for the polygon.

    The rectangle variant is described by the members "mMatrix", "mWidth", and "mHeight".
    The polygon is described by the member "points".

    \todo
    The polygon functionality in the class is not used any more. It was partially moved
    to the ZFPolygon class. ZFPolygon objects can not yet be used when creating ortho
    photos while the ZFPlane in polygon mode could be used. Some of this functionality
    still needs to be moved from here to the ZFPolygon. When loading objects from the
    project xml file, ZFPlane objects in the polygon mode must be loaded as ZFPolygon
    objects.
    */
    class ZFSPRJ_API ZFPlane : public ZFPrjObj
    {
        DEFINE_ZFBASE(ZFPrjObj)

    public:
        enum flags_t {
            F_UNDEF = 0,
            F_TEMP = 1
        };

        enum plane_t {
            PLANE_HESSE,		///< nur hesse form ohne width/height
            PLANE_HESSE_WH,		///< hesse form mit width + height
            PLANE_POLY			///< Polygonzug
        };

        static const double fillTransparency;   ///< The transparency of the interior of the plane when its drawn.

    protected:
        ZFTRMatrix mMatrix;     /**< Upper-Left 3x3 matrix is the rotated coordinate system. The z-vector is the plane-normal. x,y are parallel to the plane edges. p ist the translation vector to the center of the plane. It transforms points from plane space to world space.
                                // x1 y1 z1 p1      Indizes:  0  1  2  3       ( in m wenn using getSbMatix)
                                // x2 y2 z2 p2                4  5  6  7       ( NOTE! you will get the transposed (here also inverted) matrix when using getZFMatrix() and then GetTRMatrix4x4( m)
                                // x3 y3 z3 p3                8  9 10 11
                                //  0  0  0  1               12 13 14 15 */
        plane_t typ;

        double mHeight;			///< hesseform height, edgelength in x direction
        double mWidth;			///< hesseform width, edgelength in y direction
        bool ok;
        bool mIs2D;				///< Worldfile posible

        plane_points_t	points;	///< die Punke der Flaeche (polygon) in welt koordinaten.

        double dUmfang;			///< Umfang der Plane
        double dFlaeche;		///< Flaecheninhalt

        // statistic / quality
        int pointsOnPlane;		///< punkte die auf der flaeche liegen
        int pointsOutOfPlane;	///< punke die von der Flaeche umschlossen werden aber nicht auf der flaeche liegen
        double dStd;			///< standardabweichung der punkte zu der flaeche

        flags_t flags;

    public:
        ZFPlane() : ZFPrjObj( NULL ) { init(); }
        ZFPlane(ZFPrjObjNode*,  tinyxml2::XMLNode* node);
        ZFPlane(ZFPrjObjNode*, const wchar_t* layer, const wchar_t* name, double width, double height, const ZFTRMatrix& planeToWorldMatrix); ///< creates a rectangle plane
        ZFPlane(ZFPrjObjNode* p, const wchar_t* name, plane_points_t& polyPoints, const double* preferedNormalDirection = NULL);              ///< creates a polygone plane
        virtual ~ZFPlane() {}

        void init();

        std::string Xml() const override { return "Plane"; }
        virtual const QPixmap* GetPixmap();

        static void clearPixmap();

        bool readValues( tinyxml2::XMLNode* node);             ///< Initialize the data members of the plane by reading them from node.
        bool writeValues( tinyxml2::XMLNode* node);            ///< Write the plane data into node.
        ZFTRMatrix getZFMatrix() const { return mMatrix; }
        void getSbMatrix(double* m) const;                  ///< plane space transformation with translation
        double getHeight() const { return mHeight; }
        double getWidth() const { return mWidth; }
        const wchar_t* getDrawName() const;                   ///< Rueckgabe des namens

        flags_t getFlags() const { return flags; }
        void setFlags(flags_t f) { flags = (flags_t)((int)flags | f); }
        void delFlags(flags_t f) { flags = (flags_t)((int)flags & ~f); }

        int getHesse(double &dist, double& n0, double& n1, double& n2) const;

        static void exportHead(FILE* fpPick);
        static void exportVRML(bool writeHead, FILE* fpPick, const QString& picName,
            double roX, double roY, double roZ, 
            double loX, double loY, double loZ, 
            double ruX, double ruY, double ruZ, 
            double luX, double luY, double luZ,
            double rotation = 0.0);

        void polyClr();
        int polyAdd(const ZFPlanePolyPnt&);   ///< This will also set the type to POLY_PLANE
        int polyDelLast();
        int polyPoints();                                   ///< Get the number of points
        plane_points_t getPoly() const;                     ///< Points are in world coordinates.
        void setPoly(const plane_points_t& newPoints);     ///< Set new points. This will also set the type to POLY_PLANE and recompute width, height and the matrix
        plane_points_t getLocalPoly() const;                ///< Returned points are in the local system defined by mMatrix

        double getCircumference() const;
        double getArea() const;

        void setStat(double std, int pointsOnPlane, int pointsOutOfPlane);
        int getPointsOnPlane() const;
        int getPointsOutOfPlane() const;
        double getStd() const;
        int getTyp() const { return typ; }                      ///< Width and Height or Polygon

        void flipZAndYInPlaneMatrix();                  ///< Flip the z and y axis in the plane system
        void rotateXAndY90DegAroundZ();                 ///< Rotate the plane system 90 deg around normal

        /**
        \brief Rotate the system around the z-axis that the x-axis will be closer to the global horizontal
        and the y-axis will be closer to the global z-axis.

        \param eZ = is assumed to be the plane normal
        The length of the vectors will be conserved
        */
        static void rotatePlaneBasisForAGoodOrthoFotoDefault(ZFVec3d& uX, ZFVec3d& uY, ZFVec3d& uZ);
        /**
        eX, eY, eZ should be of length 3, resultBasis is of length 9 with the arrangement:
        res[0] res[1] res[2]
        res[3] res[4] res[5]
        res[6] res[7] res[8]
        So the resulting eX = ( res[0], res[3], res[6] )
        */
        static void rotatePlaneBasisForAGoodOrthoFotoDefault(const double* eX, const double* eY, const double* eZ, double* resultBasis); /** \overload */
        static void rotatePlaneBasisForAGoodOrthoFotoDefault(double* uX, double* uY, double* uZ); /** \overload */


    protected:
        void calculateCircumfenceAndArea();		        ///< Berechnet umfang und flaeche, ...

    private:
        double calculatePolygonCircumfence();
        double calculatePolygonArea();
        //void sortPolyVectorsIntoTrianglesWithMinimalAngle( std::vector<ZFPlanePolyPnt>& polyVectors);
        void calculatePolyPointsFromWidthAndHeight();
        void calculateWidthAndHeightAndTransformationmatrixFromPolyPoints(const double* preferedNormalDirection);
        void calculateWidthAndHeightAndTransformationmatrixFromPolyPointsWithCurrentZ();    // uses the current z direction as preferedNormaldirection.
        bool flipPlaneNormalAccordingToPreferredDirection(const double* preferedNormalDirection, double* planeNormal);
        void computePlaneCoordinateSystemForPlanePointsAndWidthAndHeight(const plane_points_t& points, const double* preferedNormalDirection, double* bX, double* bY, double* bZ, double* planeCenter);  /// Computes an orthonormal basis the lies within the 2D-plane that is created with the points in points.
    };

    //=====================================================================================================
    /// A node that holds all the ZFPlane objects in a project.
    class ZFSPRJ_API ZFPlanes : public ZFPrjObjNode
    {
        DEFINE_ZFBASE(ZFPrjObjNode)

    private:
        static int nameCounter;                 ///< This is used for generating the default name of a plane.
        static std::wstring defaultNamePrefix;  ///< This is used for generating the default name of a plane.

    public:
        ZFPlanes(ZFProject*);

        virtual bool Load();
        virtual bool Store(double versNr);

        QString Typ() const override { return "Planes"; }
        std::string Xml() const override { return "Planes"; }

        void addZPlane(QString name = "");
        void addPlane(const std::wstring& layer, const std::wstring& name, double width, double height, ZFTRMatrix m);
        ZFPlane* addPlane(plane_points_t& polyPoints, const double* preferedNormalDirection = NULL);
        void addPlane(ZFPlane* plane);                             ///< Ownership of plane goes to planes. The name of plane will be changed if there is already a plane with that name in planes.
        ZFPlane* getPlane(const std::wstring& name);
        ZFPlane* takePlaneFromPlanes(const std::wstring& name);    ///< ownership of the plane goes to the caller. Plane will be removed from planes.
        bool deletePlane(const std::wstring& name);

        void Cleanup();

        static void setDefaultNamePrefix(std::wstring prefix) { defaultNamePrefix = prefix; }
        static std::wstring getDefaultName();

    protected:
        void putPlaneIntoPlanes(ZFPlane* plane);
    };

    //=====================================================================================================
} // end namespace zfprj
