#pragma once
#include "../zfsapi.h"

#include "../file_extensions.h"

#include <string>

// path where to store filter params and filter results
#define FILTER_PATH     folder_zfs_filter



namespace zfs
{
    class FilterParams;

    /// create a filename for read/write filter params. Will create directory if not exist
    ZFS_API std::wstring filterParameterFile(const std::wstring& zfsFileName);


    // create a filename for storing filter result. Will create directory if not exist
    ZFS_API std::wstring filterResultFile(const std::wstring& zfsFileName);



    /**
     * \brief Set dirty flag for filter after params modified to tell that scans has to be re filtered.
     */
    ZFS_API void setZfsFilterDirty(const std::wstring& zfsFileName, bool dirty=true, bool mixpixenh=false);
    ZFS_API bool isZfsFilterDirty(const std::wstring& zfsFileName);
    ZFS_API void setZfsFilterDirty(const std::wstring& zfsFileName, std::shared_ptr<FilterParams> orgprm, bool dirty = true);
    ZFS_API bool isZfsFilterDirty(const std::wstring& zfsFileName, std::string key);


}
