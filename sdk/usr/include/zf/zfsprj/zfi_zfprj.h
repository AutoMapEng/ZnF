#ifndef ZFPRJ_ZFI_H
#define ZFPRJ_ZFI_H

#include "ZfsPrjBasic.h"

#include <zfs/zfi/zfi_zfs.h>
#include <zfs/Tools/progress.h>


#include <vector>

#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <QStringList>
SWITCH_WARNINGS_ON

class QImage;
class ZFProject;

namespace zfprj
{

    /**
        Qt Version of ZfiList
    */
    ZFPRJ_API QStringList ZfiList( const QString& mpo, bool absolutepath, int id=ZFI_ID );
    ZFPRJ_API QStringList ZfiListImages(const QString& mpo, bool absolutepath);

    /**
        Qt Version of ZfiList
    */
    ZFPRJ_API QStringList ZfiExtract( const QString& mpo, const QString& subfolder="Color", zf::progress_f cb=nullptr );

    /**
        Load Image into QImage
    */
    ZFPRJ_API QImage* ZfiLoadImageQt( const QString& zfi, const QString& ifn );

    /**
        Load Images into QImages
    */
    ZFPRJ_API std::vector<QImage*> ZfiLoadImagesQt( const QString& zfi, QStringList& images );

    /**
        List of images which belongs to one HDR serie
    */
    ZFPRJ_API QStringList ZfiHdrSerie( const QString& zfi, int id );
    ZFPRJ_API QStringList ZfiHdrSerie(const QString& zfi, const QStringList& images, int id);

    /**
        HDR Result Image Names of img serie
    */
    ZFPRJ_API QStringList ZfiHdrResultNames( const QString& zfi);

    /**
        Add zfi file to a zfs scan
    */
    ZFPRJ_API bool Zfi_Scan2Project( ZFProject& p, const QString& scan, const QString& zfi );

} // end namespace

#endif // ZFPRJ_ZFI_H
