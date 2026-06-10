#ifndef _ZFS_CHECK_H
#define _ZFS_CHECK_H

#include "zfsapi.h"
#include "config.h"

#ifdef ZFS_5003_SUPPORT

#include <string>

class ZFScanZFS;

enum // returncodes
{ 
    HAT_UNDEF,          // undefined error
    HAT_WRONGZFS,       // wrong type of zfs file. Must be imager scan
    HAT_OK,             // -->> Glas is clean
    HAT_NOPARAM,        // no params defined in imager for check
    HAT_WRONGPARAM,     // params not ok
    HAT_NOFILE,         // no zfs file
    HAT_NOHAT,          // no hat found in scan
    HAT_TOLESSPIXEL,    // too less pixels in scan
    HAT_DIST,           // distance of hat wrong
    HAT_WIDTH,          // width of had wrong
    HAT_NOT180DEG,      // hat not mounted correctly
    HAT_RFTOLESS,       // too less pixels for referenz measurement
    HAT_GLASDIRTY,      // -->> glas is dirty
    HAT_TOLESSLINES,    // too less lines in scan too execute check

    _HAT_
};

ZFS_API_F const char* CheckHat_ErrorString( int result );

#ifdef HATCHECK
ZFS_API_F double CheckHat_GetValue( int idx ); // read back internal values required for setup hat
ZFS_API_F std::string CheckHat_ScanParam(); // required params for hat check you have to add mor params by yourself like " -file <name>"
ZFS_API_F int CheckHat( ZFScanZFS* zfs, std::string& log, const char* param ); // check if gas is clean
#endif
#endif // ZFS_5003_SUPPORT

#endif // _ZFS_CHECK_H
