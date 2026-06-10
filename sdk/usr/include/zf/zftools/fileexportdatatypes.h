#ifndef FILEEXPORTDATATYPES_H
#define FILEEXPORTDATATYPES_H

#include <string>
#include <vector>


namespace zftools {

//#############################################################################
/**
\brief helper class for global DXFFile write functions
*/
    class ZFVec3dDxf {
        std::string name;

    public:
        bool point;
        double x, y, z;

        ZFVec3dDxf() : point(false), x(0), y(0), z(0) {}
        ZFVec3dDxf(double xArg, double yArg, double zArg, 
            const std::string& _name = "") :
            name(_name), point(false), x(xArg), y(yArg), z(zArg) {
        }

        const std::string& getName() const {
            return name;
        }

        void setName(const char* n) {
            name = n;
        }

        void setName(std::string n) {
            name = n;
        }
    };

    //#############################################################################
    /**
    \brief helper class for global DXFFile write functions

    */
    class DxfLayer {
    public:
        std::string name;
        int color;
        std::vector< std::vector<ZFVec3dDxf> > lstlst; // a vector with polylines as elements.
        std::vector< std::vector<ZFVec3dDxf> > lstlstIt;

        DxfLayer() {
            name = "";
            color = 0;
        }
    };

    //########################### Class DxfFileStringDefines ###################
    /**
    \brief This class contains the definition of the strings which are used to compose a dxf file.
    This is a helper class for the DxfFileWriter
    */
    class DxfFileStringDefines {
    public:
        const char* DXFGenerated;
        const char* DXFSection;
        const char* DXFTables;
        const char* DXFTable;
        const char* DXFLayerdef;
        const char* DXFMycolor;
        const char* DXFMylayerdef;
        const char* DXFLinetype;
        const char* DXFEndtab;
        const char* DXFEndsec;
        const char* DXFPoint;
        const char* DXFLine;
        const char* DXFTEXT;
        const char* DXFTEXTDEF;
        const char* DXFPolyline;
        const char* DXFPolylinelayer;
        const char* DXFFace;
        const char* DXFVertex;
        const char* DXFMylayer;
        const char* DXFEntities;
        const char* DXFSeqend;
        const char* DXFEnd;


        DxfFileStringDefines();
    };

    //########################### Class DxfFace ###############################
    /**
    \brief Three points that form a triangle
    */
    class DxfFace {
    public:
        ZFVec3dDxf p1;
        ZFVec3dDxf p2;
        ZFVec3dDxf p3;
    };

    //################## Class LayerDxfFileWriter #############################
    /**
    \brief helper class for the DxfFileWriter
    This class holds the information that is printed in the dxf file.
    */
    class LayerDxfFileWriter {
    private:
        std::string name;

    public:
        int color;
        std::vector<zftools::ZFVec3dDxf> mPoints;                       // This holds single 3D points that can also have a name.
        std::vector<zftools::ZFVec3dDxf> textlabels;        // This holds positions of text labels.
        std::vector<std::vector<zftools::ZFVec3dDxf> > polygonalLines;   // This holds polygonal lines.
        std::vector<DxfFace> faces;

        LayerDxfFileWriter();

        std::string getName() { return name; }
        void setName(std::string name);
    };

} // namespace

#endif // !FILEEXPORTDATATYPES_H
