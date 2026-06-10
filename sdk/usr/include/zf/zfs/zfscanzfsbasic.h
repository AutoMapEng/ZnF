#pragma once

#include "config.h"

#include <string>

#ifdef ZFS_UK_CALIB
class ZFSensorModel;
#endif
class ZFSHeader;
class ZFScanZFS;

#define N_MEM   8   // number of memory maps



class ZFScanZFSBasic
{
    friend class ZFScanZFS;

protected:
    ZFScanZFS* parent{};
    std::wstring     fileName;
#ifdef ZFS_UK_CALIB
    ZFSensorModel* pSensorModel{};
#endif
    ZFSHeader*     pHeader{};

    int subsample{1};

    // from header
    int typ{};
    int bytesPerPixel{};
    int pixel{};
    int lines{};
    int lineHeaderSize{};
    int pixel360{};
    int lines360{};

    int   dataSize{};
    int	  lineSize{};

    // ZFger calib
public:
    double cx{};
    double cy{};
    double cz{};
    double oH{};

    // 1001
    double a0{},a1{}, b0{},b1{},b2{},b3{};

    // 1004
    //double params_lenc[2];
    //double params_henc[2];
    //double params_AzimSin[2];
    //double params_ElevSin[2];
    double params_ElevDroop[3]{};
    //double params_AzimRange[3];
    //double params_Level[5];

public:
    ZFScanZFSBasic( ZFScanZFS* );
    virtual ~ZFScanZFSBasic();

    bool Open( const std::wstring& filename );
    void Close();

    const wchar_t* FileName() { return fileName.c_str(); }

    int BytesPerPixel() const { return bytesPerPixel; }
    int LineHeaderSize() const { return lineHeaderSize; }
    int LineSize() const { return lineSize; }
    int DataSize() const { return dataSize; }
    ZFSHeader* Header() { return pHeader; }
    const ZFSHeader* Header() const { return pHeader; }

#ifdef ZFS_UK_CALIB
    ZFSensorModel* SensorModel() { return pSensorModel; }
#endif
    int Pixel() const { return pixel; }
    int Lines() const { return lines; }
};

