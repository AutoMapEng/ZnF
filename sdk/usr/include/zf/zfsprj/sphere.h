#ifndef _ZFPRJ_SPHERE_H_
#define _ZFPRJ_SPHERE_H_

/**
    classes holding spheres stored in project
*/

#include "ZfsPrjBasic.h"
#include "obj.h"


class ZFProject;

namespace tinyxml2 {
    class XMLNode;
}

namespace zfprj
{
    //=====================================================================================================


    class ZFSPRJ_API ZFSphere : public ZFPrjObj
    {
        DEFINE_ZFBASE(ZFPrjObj)

    protected:
        double x, y, z;   // position
        double r;       // radius

    public:
        ZFSphere(ZFPrjObjNode*,  tinyxml2::XMLNode* node);

        std::string Xml() const override { return "Sphere"; }

        bool readValues( tinyxml2::XMLNode* node);
        bool writeValues( tinyxml2::XMLNode* node);
    };


    //=====================================================================================================


    class ZFSPRJ_API ZFSpheres : public ZFPrjObjNode
    {
        DEFINE_ZFBASE(ZFPrjObjNode)

    protected:

    public:
        ZFSpheres(ZFProject*);

        virtual bool Load();
        virtual bool Store(double versNr);
        std::string Xml() const override { return "Spheres"; }

    protected:
    };

    //=====================================================================================================


    //=====================================================================================================
} // end namespace zfprj

#endif //_ZFPRJ_SPHERE_H_
