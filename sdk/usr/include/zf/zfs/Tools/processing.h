#pragma once

/**
    Interface for processing status of a scan like filtered or targets searched in the scan
    For every process status a file will be created
*/

#include "../zfsapi.h"

#include <string>


namespace zfs
{
    /**
    \brief creates a flag which tells that scan has a state like filtered
    \scan alsolute file name of the scan
    \key a key identifieing the process like "filtered"
    */
    bool ZFS_API createProcessingFlag(const std::wstring& scan, const std::wstring& key );

    /**
    \brief Check if scan was processed
    */
    bool ZFS_API check4ProcessingFlag(const std::wstring& scan, const std::wstring& key);


    /**
    \brief clear the processing flag
    */
    bool ZFS_API clearProcessingFlag(const std::wstring& scan, const std::wstring& key);

} // end namespace zfs


