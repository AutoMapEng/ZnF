#ifndef _ZFPRJ_REGISTER_H_
#define _ZFPRJ_REGISTER_H_

/**
    classes for registration
*/

#include "ZfsPrjBasic.h"
#include "obj.h"

#include "../zfs/zfs.h"

#include "coordinatesystems.h"

class QString;


namespace zfprj
{

class ZFSPRJ_API ZFRegister : public ZFPrjObjNode
{
    DEFINE_ZFBASE(ZFPrjObjNode)

protected:
    std::shared_ptr<CoordinateSystems> coordinateSystems;

public:

public:
    ZFRegister( ZFProject* );

    virtual bool Load();
    virtual bool Store( double versNr );
    QString Typ() const override { return QObject::tr("Register"); }
    std::string Xml() const override { return "RegisterWorld"; }  // xml sektion where to store

    // bool SetXYZCallback( ZFScan*, bool check );

    void Cleanup(); // alles loeschen

    std::shared_ptr<CoordinateSystems> getCoordinateSystems() const {
        return coordinateSystems;
    }
};


} // end namespace zfprj

#endif //_ZFPRJ_REGISTER_H_
