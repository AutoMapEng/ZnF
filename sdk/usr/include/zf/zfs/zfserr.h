#pragma once

#include "zfsapi.h"
#include <stddef.h>

//##################################### ZFS Info ##################################################

namespace zfs
{
    enum zfsInfo_t
    {
        INFO_APPL_MIX_PIX_FILTER,
        INFO_SCAN_LOAD,
        INFO_LOAD,
        INFO_READ_MASK,
        INFO_LOAD_COLOR_IMG,
        INFO_DETECT_VIEW_MODE,
        INFO_LOAD_PTX,
        INFO_APPL_CRITICAL_COMPENSATION_FILTER,
        INFO_APPL_LEVELSYSTEM,          ///< no information about the level system stored in the zfs header
        INFO_APPL_COMPENSATOR,          ///< dynamic compensator available
        INFO_APPL_COMPENSATOR_OFF,      ///< dynamic compensator disabled by the user
        INFO_APPL_LEVEL                 ///< level available

    };

    //========================================================================================================

    ZFS_API_F const wchar_t* ZFSInfoString(zfs::zfsInfo_t);   ///< Convert inofnumber to a string

    typedef const wchar_t* (*info_cb_f)(zfs::zfsInfo_t);
    ZFS_API_F void ZFSInfoInstallCallback(info_cb_f);        ///< install a callback for infostringmapping for language support


    //##################################### ZFS Error #################################################

    /**
    \brief    Errorcodes while open / access zfs files
    */
    enum zfserr_t
    {
        ERR_OK,                     ///< success

        ERR_UNDEF,                  ///< undefined error
        ERR_LICENSE,                ///< no valid license
        ERR_OPEN,                   ///< error on open file
        ERR_OUTOFMEM,               ///< out of memory
        ERR_OUTOFMEM_IMAGE,         ///< out of memory loading
        ERR_READMASK,               ///< problem reading mask / may out of memory
        ERR_UNKNOWNFORMAT,          ///< not a zfs file
        ERR_ZFSHEADERTYP_NOTSUPPORTED, ///< the zfsheader has a newer typ, please update software
        ERR_TYPNOTSUPPORTED,        ///< datatyp of the file not supported, please update library
        ERR_CALIBNOTSUPPORTED,      ///< calibtyp not supported, please update software
        ERR_CORRUPTED,              ///< file corruped, please try repair
        ERR_CRASH,                  ///< internal problem
        ERR_WRITEPROTECT,           ///< write protected
        ERR_MISSNEXT,               ///< miss next part of zfs in a linked list
        ERR_PARAM,                  ///< not supported param for open scan
        ERR_USERABORT,              ///< abort by user
        ERR_COLORFILE_CORRUPTED,    ///< colorimage corrupted please delete image
        ERR_COLORFILE_LOADFAILED,   ///< colorimage failure on load
        ERR_SOFTWARETOOLD,          ///< zfs.dll to old to load file
        ERR_SCANFILE_DOES_NOT_EXIST,///< scan file does not exist
        ERR_HEADER_FULL,			///< no space in scan header availabel
        ERR_ALREADY_OPEN,           ///< scan is already open

        _ERR_
    };
    //========================================================================================================

    ZFS_API_F const wchar_t* ZFSErrorString(zfs::zfserr_t);  ///< Convert errornumber to a string

    typedef const wchar_t* (*error_cb_f)(zfs::zfserr_t);
    ZFS_API_F void ZFSErrorInstallCallback(error_cb_f);   ///< install a callback for errorstringmapping for language support

    //========================================================================================================
} // end namespace zfs
