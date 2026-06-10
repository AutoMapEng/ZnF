#pragma once

#include "zfsapi.h"

#include <string>
#include <vector>

class ZFRect;
class ZFScan;
class ZFSHeader;
class ZFTRMatrix;
class ZFScanZFS;
namespace zf
{
class FileName;
}



class ZFS_API ZFSUncal
{
    ZFScanZFS* zfs = nullptr;
    bool cal = false;
    double hoffset = 0;
    double loffset = 0;
    double moffset = 0;
    double mscale = 0;

public:
    ZFSUncal( ZFScan*, bool rg=true ); // rg = switch rangel offset/scale also off
    ~ZFSUncal();
};


ZFS_API ZFRect createRect( int pixel, int lines, int x, int y, int offset ); // create a rectangle around x,y with offset, checks limits
ZFS_API ZFRect getSelectionRect( ZFScan* z ); // get first selection or whole image
//ZFS_API bool UpdateHeader( const std::wstring& fn, bool (*u)(ZFScan*,ZFSHeader*,void*), void* ptr=NULL ); // update scan header (scan can be open or not)



//================================================================================================================

/// read register info from header to matrix class (internaly 280 is used, sdk uses typically 320 which is including the coordinatesystem)
enum class read_registration_t : int { REG_320_FINAL, REG_280 };
ZFS_API bool readRegistration( const ZFSHeader* h, ZFTRMatrix* tr, read_registration_t= read_registration_t::REG_280);

/// read register used for external programms defined in 320...
ZFS_API ZFTRMatrix readRegistration320(const ZFSHeader& h);
ZFS_API ZFTRMatrix readRegistration320(const ZFSHeader* h);

//================================================================================================================

ZFS_API bool ZFSDLL_HistogramNormalize( zf::FileName &fn, bool rotorcam = false ); /// Histogram normalize for png and jpg images

/**
\brief ZFSHasColorFile checks if there is a color scan for the scan available
*/
ZFS_API bool ZFSHasColorFile( const std::wstring& zfsFileName );

/**
\brief ZFSColorFile
\return the colorfilename if one exist
*/
ZFS_API std::wstring ZFSColorFile(const std::wstring& zfsFileName);
ZFS_API std::vector< std::wstring > ZFSColorFiles(const std::wstring& zfsFileName);

/**
\brief ZFSRemoveColorFile remove all color files attached to the scan
\return true if a file is removed
*/
ZFS_API bool ZFSRemoveColorFile(const std::wstring& zfsFileName);


//##############################################################################################

ZFS_API std::wstring ZFSColorPanorama_FindFileWithBestResolution(const std::wstring& absZfsName, const std::wstring& searchInSubfolder=L"Export" );

/**
\brief ZFSColorPanoramaFile
\return the colorpanoramafilename if one exist
*/
ZFS_API std::wstring ZFSColorPanoramaFile(const std::wstring& zfsFileName);
ZFS_API std::vector< std::wstring > ZFSColorPanoramaFiles(const std::wstring& zfsFileName);

/**
\brief ZFSColorPanoramaFileName
\return the colorpanoramafilename
*/
enum highrespano_t { HRP_UNDEF, HRP_H, HRP_SH, HRP_USERDEFINED, _HRP_ };
ZFS_API std::wstring ZFSColorPanoramaFileName( const std::wstring& zfsFileName, highrespano_t res);

/**
\brief ZFSRemoveColorPanoramaFile remove all color files attached to the scan
\return true if a file is removed
*/
ZFS_API bool ZFSRemoveColorPanoramaFile(const std::wstring& zfsFileName);

/**
\brief GetcolPanoramaNamePrefix shows the used reference name for colPanorama files
\return the used reference name for colPanorama files
*/
ZFS_API std::wstring GetcolPanoramaNamePrefix();


namespace zfs
{

    ZFS_API bool IsProfiler(const ZFSHeader* );              ///< return true if scan was taken in profiler mode or scanner is a profiler

} // end namespace

