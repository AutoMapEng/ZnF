#pragma once
#include "zfsapi.h"

#include <string>


namespace zfs
{
    /**
    \brief return file name of the mapped infrared data
    */
    ZFS_API std::wstring infraredFileNameOfScan(const std::wstring& absZfsFileName);
        
    /**
    \brief detect wheter scan has mapped infrared data
    */
    ZFS_API bool hasScanInfrared(const std::wstring& absZfsFileName );

}