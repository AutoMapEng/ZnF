#ifndef __ZFS_EXTENSION_H__
#define __ZFS_EXTENSION_H__

#include "zfsapi.h"
#include "zfserr.h"
#include <zfcore/filename.h>

#include <string>
#include <vector>

class ZFScan;

typedef ZFScan* (*fileopen_f)( const std::wstring& filename, const char* param, int openflags, zfs::zfserr_t* );


class ZFS_API ZFFileExtension
{
protected:
    std::wstring sext;
    std::string sinfo;
    std::string shelp;

public:
    fileopen_f cbopen{};  // callback function

public:
    ZFFileExtension( fileopen_f, const wchar_t* ext, const char* info, const char* help=NULL );
    virtual ~ZFFileExtension() = default;

    const wchar_t* ext() { return sext.c_str(); }
    const char* info() { return sinfo.c_str(); }
    const char* help() { return shelp.c_str(); }
};



typedef std::vector<ZFFileExtension> fileextension_t;



class ZFS_API ZFScanFilesSupport
{
public:
    fileextension_t fileextension;

public:
    ZFScanFilesSupport() = default;
    virtual ~ZFScanFilesSupport() = default;

    void addFileFormat( fileopen_f, const std::wstring& ext, const char* info, const char* help=NULL );
    void addFileFormat( fileopen_f, const std::string& ext, const char* info, const char* help=NULL );
    void clear();
};




extern ZFS_API ZFScanFilesSupport scanFilesSupport;


#endif //__ZFS_EXTENSION_H__
