#ifndef _ZFPRJ_SHOOTCHANNEL_H_
#define _ZFPRJ_SHOOTCHANNEL_H_

/**
    classes for shootchannel
*/

#include "ZfsPrjBasic.h"
#include "obj.h"

#include <string>

namespace zfprj
{
class ZFShootChannels;

//=============================================================================================
/// An object that represents the trajectory of a bullet in a crime scene.
class ZFSPRJ_API ZFShootChannel : public ZFPrjObj
{
    DEFINE_ZFBASE(ZFPrjObj)

protected:
    int mID;
    double mStartX;
    double mStartY;
    double mStartZ;
    double mEndX;
    double mEndY;
    double mEndZ;
    double mBulletHoleX;
    double mBulletHoleY;
    double mBulletHoleZ;
    double mBulletHoleoffset;
    double mTransValue;
    double mConeAngel;
    double mLength;
    double mLegend;
    int mLineSize;
    bool mShow;
    bool mFlipCone;
    bool mLine;
    bool mCone;
    bool mTrans;

public:
    int mR = 255;
    int mG = 0;
    int mB = 0;

public:
    ZFShootChannel( ZFShootChannels* p);
    ZFShootChannel( ZFShootChannels* p, int id, const std::wstring& name, 
        double bulletHoleX, double bulletHoleY, double bulletHoleZ,
        double fpX, double fpY, double fpZ, 
        double endX, double endY, double endZ, 
        bool useTrans, double trans, double coneAngel,
        bool line, double length, bool cone, bool show, double legend, bool flip, int lineSize,
        int r, int g, int b, bool global );

    std::string Xml() const override { return "ShootChannel"; }
    virtual const QPixmap* GetPixmap();

    void getID(int& id) const { id = mID; }
    void getStartPoint( double& x, double& y, double& z, bool global ) const;
    void getBulletHole( double& x, double& y, double& z, bool global ) const;
    void setBulletHole( double x, double y, double z, bool global );
    void getBHOffset( double& bho) const {bho = mBulletHoleoffset;}
    void getEndPoint( double& x, double& y, double& z, bool global ) const;
    void setEndPoint( double x, double y, double z, bool global );
    bool drawLine() const { return mLine; }
    bool drawCone() const { return mCone; }
    double getTrans() const
    {
        return mTransValue;
    }
    void setShow( bool bo ) { mShow = bo; }
    bool drawTrans() const { return mTrans; }
    double getConeRad() const { return mConeAngel; }
    double getLength() const { return mLength; }
    double getLegend() const { return mLegend; }
    bool show() const { return mShow; }
    bool getFlip() const { return mFlipCone; }
    int getLineSize() const { return mLineSize; }
};

//=============================================================================================
//static int nameCounter; ///< This is used for generating the default name of a plane.

/// A node that holds all the ZFShootChannel objects in the project.
class ZFSPRJ_API ZFShootChannels : public ZFPrjObjNode
{
    DEFINE_ZFBASE(ZFPrjObjNode)

private:
    static int nameCounter;                 ///< This is used for generating the default name of a plane.
    static std::wstring defaultNamePrefix;  ///< This is used for generating the default name of a plane.

public:
    ZFShootChannels( ZFProject* );

    virtual bool Load();
    virtual bool Store( double versNr );
    QString Typ() const override { return QObject::tr("Shoot channels"); }
    std::string Xml() const override { return "ShootChannels"; }  // xml sektion where to store

    ZFShootChannel* AddShootChannel(const std::wstring& name, double bulletHoleX, double bulletHoleY, double bulletHoleZ,
                         double fullPenetrationX, double fullPenetrationY, double fullPenetrationZ,
                         double endX, double endY, double endZ,
                         bool useTrans, double trans, double coneAngel, bool line,
                         double length, bool cone, bool show, double legend, bool flip, int lineSize,
                         int r, int g, int b);

    bool DelShootChannel( const std::wstring& name );

    ZFShootChannel* FindShootChannelByText( const std::wstring& text );
    bool deleteObj( const std::wstring& text );

    std::wstring getNewName( const std::wstring& languagetext=L"" );

    void Cleanup();

    static void setDefaultNamePrefix( std::wstring prefix){ defaultNamePrefix=prefix;}

};

//=============================================================================================


} // end namespace zfprj

#endif //_ZFPRJ_LINKS_H_
