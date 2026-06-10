#pragma once

#include "zfsapi.h"

#include <stdint.h>
#include <string>
#include <functional>
#include <utility>

#include "mask.h"
#include "zfserr.h"

class ZFSHeader;
class ZFBase;
namespace zfs { namespace math { class rgb; } }


enum mask_id_t
{
    MASK_ID_GLOBAL = 5,
    MASK_ID_NOTGLOBAL = 6
};

namespace zfs
{
    /**
    @brief direct reading mask and call callback with the mask to add to a scan
    @return 0=if there are no masks, >0 number of masks read succesfully, <0 failed
    */
    ZFS_API 
    int readMasksFromScan(const ZFSHeader* h, const std::wstring& zfsAbsFileName, ZFBase* context, 
        std::function< std::pair<bool,ZFMask*>(ZFBase* context, uint32_t type, uint32_t wd, uint32_t hg, const char* data, const zfs::math::rgb&, const std::wstring& name)> );

    /**
    @brief all existing masks will be erased. Afterwards all masks in the list will be written to the scan. 
           zfsheader has to be writen afterwards
    */
    ZFS_API 
    int writeMasks2Scan(ZFSHeader* h, const std::wstring& zfsAbsFileName, const mask_lst& lst, zfs::zfserr_t& err);



    /// did not need to open the scan itself
    ZFS_API
    bool addMasks(const std::wstring& zfsfn, const std::vector<ZFMask*>& masks);




    /// did not need to open the scan itself

    /**
    *   Remove all masks from the scans in the zfsfns list
    *   Changes zfsheader as well
    *   \param zfsfns = List with all filnames with path to remove masks
    *   \param removeFixedAsWell = should fixed masks be deleted as well
    */
    ZFS_API 
    bool removeAllMasks( const std::vector<std::wstring>& zfsfns, bool removeFixedAsWell );

    /**
    *   Remove all masks from the scan
    *   Changes zfsheader as well
    *   \param zfsfns = List with all filnames with path to remove masks
    *   \param removeFixedAsWell = should fixed masks be deleted as well
    */
    ZFS_API
    bool removeAllMasks(const std::wstring& zfsfn, bool removeFixedAsWell);



    /**
    *   Remove masks from the list from the scans in the zfsfns list
    *   Changes zfsheader as well
    *   \param zfsfns = List with all filnames with path to remove masks
    *   \return true if any changes are done (masks removed)
    */
    ZFS_API
    bool removeMasks( const std::vector < std::wstring>& zfsfns, const std::vector<std::wstring>& masks );

    /**
    *   Remove masks from the list from the scan
    *   Changes zfsheader as well
    *   \param zfsfns = List with all filnames with path to remove masks
    */
    ZFS_API
    bool removeMasks(const std::wstring& zfsfn, const std::vector<std::wstring>& masks);


    /**
    @brief attach all masks in the list to the scan and setup it in the zfsheader.
           zfsheader will be updated
    */
    ZFS_API
        int writeMasks2otherScan(const ZFSHeader* hi, const std::wstring& srcZfsAbsFileName, const mask_lst& lst, /* source file */
            ZFSHeader* ho, const std::wstring& fname /* destination file*/);

} // end namespace
