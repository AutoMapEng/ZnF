#pragma once

#include "model.h"



//Typ14
#ifdef ZFS_MODEL_5003
class ZFS_API ZFModel5003 : public ZFModel
{
public:
    ZFModel5003(ZFSLinebyline* parent, const ZFSHeader* head);

    virtual const char* Typ() const override { return "Imager 5003"; }


protected:
    bool __Angle(int pixel, const ZFLineAccess* line, double rg, double& henc_rad, double& lenc_rad) const override;

    zfs::math::Vec3d __XYZ0(int pixel, const ZFLineAccess* line, double rg) const override;
    zfs::math::Vec3d __XYZ0( double henc, double lenc, double rg) const override
    {
        return calib->XYZ0(henc, lenc, rg);
    }
};
#endif

//-------------------------------------------------------------------------------------------------------------

//Typ16
#ifdef ZFS_MODEL_5003
class ZFS_API ZFModel5003Profiler : public ZFModel
{
public:
    ZFModel5003Profiler(ZFSLinebyline* parent, const ZFSHeader* head);

    virtual const char* Typ() const override { return "Imager 5003 Profilermode"; }

protected:
    bool __Angle( int pixel, const ZFLineAccess* line, double rg, double& henc_rad, double& lenc_rad) const override;
    zfs::math::Vec3d __XYZ0( int pixel, const ZFLineAccess* line, double rg) const override;
    zfs::math::Vec3d __XYZ0( double henc, double lenc, double rg) const override
    {
        return calib->XYZ0(henc, lenc, rg);;
    }
};
#endif

//-------------------------------------------------------------------------------------------------------------

#ifdef ZFS_MODEL_5003

//7.0.0.10 PG Klasse hinzugefuegt
class ZFS_API AngleDistanceLineInfo
{
public:
    int nr{};
    double angle{};
    double distance{};
    double enc2{};
    double x0{};
    double y0{};
    double z0{};
    double corrAng{};

    AngleDistanceLineInfo() : nr(0), angle(0), distance(0), enc2(0), corrAng(0) {}
};

//7.0.0.10 PG Klasse hinzugefuegt
class ZFS_API ZFModel5003ProfilerEnc : public ZFModel5003Profiler
{
    std::vector<AngleDistanceLineInfo> lineInfo;
    std::wstring fileName;
    double lastCentenrY{};
    double lastCenterYCor{};
    double lastCenterZCor{};

protected:
    double dist{};
    double lastValue{};
    double start{};
    double lastValueEnc3{};

    bool readFile{};

public:
    ZFModel5003ProfilerEnc(ZFSLinebyline* parent, const ZFSHeader* head);

    virtual const char* Typ() const override { return "Imager 5003 Profilermode enc"; }

    virtual bool  ReadEncFile();

    /// not thread safe
    double GetEnc2(const ZFLineAccess* line);
    double GetEnc3(const ZFLineAccess* line);

    double GetEnc3File(int line) { return lineInfo[line].angle; }
    double GetEnc2File(const ZFLineAccess* line);

    void SetReadFile(bool bo) { readFile = bo; };

protected:
    zfs::math::Vec3d __XYZ0(int pixel, const ZFLineAccess* line, double rg) const override;
    zfs::math::Vec3d __XYZ0( double henc, double lenc, double rg) const override; //ToDo

    void XYZ(int line, double&x, double& y, double& z) const;
    void RotZ(double ang, double& y, double& z, double yO, double zO) const;
    void TransRotZ(double ang, double& y, double& z, double yO, double zO) const;
    void GetAngleCorr(double Rk, double S) const;
};
#endif

//################################################################################################################

class ZFS_API ZFModel5006Enc : public ZFModel
{
public:
    ZFModel5006Enc(ZFSLinebyline* parent, const ZFSHeader* head);

    virtual const char* Typ() const override { return "Imager 5006 enc"; }


protected:
    virtual bool __Angle( int pixel, const ZFLineAccess* line, double rg, double& henc_rad, double& lenc_rad) const override;

    zfs::math::Vec3d __XYZ0( int pixel, const ZFLineAccess* line, double rg) const override;
    zfs::math::Vec3d __XYZ0( double henc, double lenc, double rg) const override
    {
        return calib->XYZ0(henc, lenc, rg);
    }
};


//-------------------------------------------------------------------------------------------------------------

class ZFS_API ZFModel5006Lara : public ZFModel
{
public:
    ZFModel5006Lara(ZFSLinebyline* parent, const ZFSHeader* head);

    virtual const char* Typ() const override { return "Imager 5006 lara"; }


protected:
    virtual bool __Angle( int pixel, const ZFLineAccess* line, double rg, double& henc_rad, double& lenc_rad) const override;

    virtual zfs::math::Vec3d __XYZ0( int pixel, const ZFLineAccess* line, double rg) const override;
    virtual zfs::math::Vec3d __XYZ0( double henc, double lenc, double rg) const override
    {
        return calib->XYZ0(henc, lenc, rg);
    }
};


//-------------------------------------------------------------------------------------------------------------


class ZFS_API ZFModel5006ProfilerEnc : public ZFModel5006Enc
{
public:
    ZFModel5006ProfilerEnc(ZFSLinebyline* parent, const ZFSHeader* head);
    virtual const char* Typ() const override { return "Imager 5006 Profilermode enc"; }

protected:
    virtual zfs::math::Vec3d __XYZ0( int pixel, const ZFLineAccess* line, double rg) const override;
    virtual zfs::math::Vec3d __XYZ0( double henc, double lenc, double rg) const override
    {
        return calib->XYZ0(henc, lenc, rg);
    }
};

//-------------------------------------------------------------------------------------------------------------

class ZFS_API ZFModel5006ProfilerLara : public ZFModel5006Lara
{
public:
    ZFModel5006ProfilerLara(ZFSLinebyline* parent, const ZFSHeader* head);
    virtual const char* Typ()const override  { return "Imager 5006 Profilermode lara"; }

protected:
    virtual zfs::math::Vec3d __XYZ0(int pixel, const ZFLineAccess* line, double rg) const override;
    virtual zfs::math::Vec3d __XYZ0(  double henc, double lenc, double rg) const override;
};


//################################################################################################################

#ifdef ZFS_MODEL_5010

class ZFS_API ZFModel5010Enc : public ZFModel5006Enc
{
public:
    ZFModel5010Enc(ZFSLinebyline* parent, const ZFSHeader* head);
    virtual const char* Typ() const override { return "Imager 5010 enc"; }
};


//-------------------------------------------------------------------------------------------------------------

class ZFS_API ZFModel5010Lara : public ZFModel5006Lara
{
public:
    ZFModel5010Lara(ZFSLinebyline* parent, const ZFSHeader* head);
    virtual const char* Typ() const override { return "Imager 5010 lara"; }
};


//-------------------------------------------------------------------------------------------------------------


class ZFS_API ZFModel5010ProfilerEnc : public ZFModel5006ProfilerEnc
{
public:
    ZFModel5010ProfilerEnc(ZFSLinebyline* parent, const ZFSHeader* head);
    virtual const char* Typ() const override { return "Imager 5010 Profilermode enc"; }
};

//-------------------------------------------------------------------------------------------------------------

class ZFS_API ZFModel5010ProfilerLara : public ZFModel5006ProfilerLara
{
public:
    ZFModel5010ProfilerLara(ZFSLinebyline* parent, const ZFSHeader* head);
    virtual const char* Typ() const override { return "Imager 5010 Profilermode lara"; }
};

//-------------------------------------------------------------------------------------------------------------

class ZFS_API ZFModel5016ProfileEnc : public ZFModel5010ProfilerEnc
{
public:
    ZFModel5016ProfileEnc(ZFSLinebyline* parent, const ZFSHeader* head);
    virtual const char* Typ() const override { return "5016 mode enc"; }

protected:
    virtual bool __Angle( int pixel, const ZFLineAccess* line, double rg, double& henc_rad, double& lenc_rad) const override;

protected:
    double fac{1.};
};

//-------------------------------------------------------------------------------------------------------------

class ZFS_API ZFModel5016ImagerEnc : public ZFModel5010ProfilerEnc
{
public:
    ZFModel5016ImagerEnc(ZFSLinebyline* parent, const ZFSHeader* head);
    virtual const char* Typ() const override { return "5016 mode lara"; }

protected:
    virtual bool __Angle( int pixel, const ZFLineAccess* line, double rg, double& henc_rad, double& lenc_rad) const override;

protected:
    double fac{ 1. };
};


//-------------------------------------------------------------------------------------------------------------

class ZFS_API ZFModel5024Enc : public ZFModel5016ImagerEnc
{
public:
    ZFModel5024Enc(ZFSLinebyline* parent, const ZFSHeader* head);
    virtual const char* Typ() const override { return "5024 enc"; }

protected:
    virtual bool __Angle(int pixel, const ZFLineAccess* line, double rg, double& henc_rad, double& lenc_rad) const override;

protected:
    int m_vertoffset{};
};

class ZFS_API ZFModel5024Lara: public ZFModel5010Lara
{
public:
    ZFModel5024Lara(ZFSLinebyline* parent, const ZFSHeader* head) : ZFModel5010Lara(parent, head) {}
    virtual const char* Typ() const override { return "5024 lara"; }
};


//-------------------------------------------------------------------------------------------------------------

#endif // ZFS_MODEL_5010

//################################################################################################################


#ifdef ZFS_MODEL_OSC
class ZFS_API ZFModelOSC : public ZFModel
{
protected:
    double resx = 0.;
    double resy = 0.;      // resolution of pixel (rad)
    double startx = 0.;
    double starty = 0.;  // start angle of first let top pixel (rad)

public:
    ZFModelOSC(ZFSLinebyline* parent, const ZFSHeader* head) : ZFModel(parent, head) {};

    virtual const char* Typ() const override { return "Color"; }

    void PreInit(const ZFSHeader*) override;

protected:
    bool __Angle( int pixel, const ZFLineAccess* line, double rg, double& henc_rad, double& lenc_rad) const override;

    zfs::math::Vec3d __XYZ0( int pixel, const ZFLineAccess* line, double rg) const override;
    zfs::math::Vec3d __XYZ0( double henc, double lenc, double rg) const override; //ToDo
};
#endif

