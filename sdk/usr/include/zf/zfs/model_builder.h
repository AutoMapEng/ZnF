#pragma once
#include "zfsapi.h"

#include <memory>

#include "zflineaccessimpl.h"
#include "model.h"

class ZFSLinebyline;
class ZFSHeader;


namespace zfs
{
    // 3line access and 3d model
    class ZFS_API ZFSLineAccessModel
    {
    public:
        void free_memory();

        ZFLineAccess* linePre{};	//!< Line bevor current line in roll mode
        ZFLineAccess* zeile{};	    //!< Current line
        ZFLineAccess* lineNext{};	//!< Line after current line. in roll mode
        ZFModel* model{};			//!< 3D Model class for xyz calculation

        std::unique_ptr<ZFLineInfo> lineInfo; ///< just if called without external LineInfo

    public:
        bool buildAccessAndModel(int typ, bool profiler, ZFLineInfo* lineInfo, const ZFSHeader* zfsHeader, ZFSLinebyline* parent = {});
        bool buildAccessAndModel( const ZFSHeader* zfsHeader);

        bool recalib(ZFSHeader* zfsHeader);
    };

}