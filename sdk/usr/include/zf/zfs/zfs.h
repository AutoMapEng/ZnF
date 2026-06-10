#pragma once

/** \mainpage C++ Library for access of Zoller+Froehlich zfs scan files

    \note Please read license.de.txt or license.en.txt stored in main directory
          before you use this software

    The base classes for access a scan are ZFSLinebyline and ZFScan.
    We recomend to use ZFSLinebyline.

    Changelog:
    \li V 7.7.0
    - support for unicode filenames as std::string (UTF8) or std::wstring (UTF16)
    \li V 7.6.0
    - released with ZFLaserControl 7.6.0
    - new class for intensity ZFRf
    \li V 7.5.0
    - require license
    \li V 7.3.10
    - new stuff for intensity correction (scan must support that, only firmware >= 7.3.0 can create such scans)
    \li V 8.0.0
    - 32/64bit
    - typ 214,215 support
    - new function XYZ2polar
*/

#include "config.h"

#include <string>
#include <memory>

#ifdef _WINDOWS
#pragma warning ( disable : 4251 )
#endif

#include <zfcore/compiler.h>
#include <zfcore/filename.h>
#include <zfcore/unicode.h>

//basic
#include "exception.h"
#include "zfdef.h"
#include "zfsdefs.h"
#include "zfsapi.h"
#include "zfbase.h"
#include "zfserr.h"
#include "zfvector.h"
#include "flags.h"
#include "file_extensions.h"
#include "zfs_data_id.h"

#include "zfsheader.h"
#include "2dselection.h"
#include "mask.h"
#include "tools.h"


// ZFScan classes
#include "zfscan.h"
#include "zfscanimpl.h"
#include "zfscantyps.h"
#include "scanlist.h"

// LineByLine
#include "linebyline.h"
#include "rf.h"

ZFS_API_F const char*	ZFSDLLVersion();	///< Version of Library
ZFS_API_F const char* ZFSDLLDate();		///< BuildDate od Library
/** required for enabling the library
 * \return 1 if ok otherwise 0
 */
ZFS_API_F int ZFSDLL_SetLicense(const char* customer, const char* expire, const char* license);

/// this is typically just for testing code and not required in a normal application
ZFS_API_F void ZFSDLL_DestroyLicense();

ZFS_API_F const char* ZFSDLL_Customer();  ///< return customer string set by ZFSDLL_SetLicense


namespace zfs
{
    /// additional info about further libraries
    ZFS_API_F
        std::string additionalVersionString();
}


/**
\brief open scanfile
\param filename = full path filename
\param param = optional params
\param flags = optional flags for open scan
\return pointer to scan. Memory must be freed by your own
*/
ZFS_API_F ZFScan* ZFSOpen(const std::wstring& filename, const char* param = NULL, int openflags = 0, zfs::zfserr_t* err = NULL);

namespace zfs
{
    ZFS_API_F std::shared_ptr<ZFScan> zfs_open(const std::wstring& filename, const char* param = nullptr, int openflags = 0, zfs::zfserr_t* err = nullptr);
}
