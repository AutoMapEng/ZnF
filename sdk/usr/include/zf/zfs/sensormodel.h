#ifndef __SENSORMODEL_H__
#define __SENSORMODEL_H__


class ZFS_EXPORT ZFSensorModel
{
    bool bInit;

public:
    ZFSensorModel();
    virtual ~ZFSensorModel() {}

    virtual void Init();
    virtual void Angle() = 0; 
    virtual void XYZ() = 0; 
};


class ZFS_EXPORT ZFSensorModel_Imager1 : public ZFSensorModel
{
public:

    virtual void Angle(); 
    virtual void XYZ(); 
};


#endif //__SENSORMODEL_H__
