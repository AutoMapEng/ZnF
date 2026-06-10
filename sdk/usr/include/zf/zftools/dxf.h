#ifndef DXF_EXPORT_H
#define DXF_EXPORT_H

/*
This file provides functionality for writing points to dxf files with global functions.
This is deprecated. Use the class DxfFileWriter in new code instead.
*/

#include "zftools_api.h"
#include <stdio.h>

#include "fileexportdatatypes.h"


namespace zftools
{


// wieder raus machen später.
ZFTOOLS_API bool DXFStart( const char* fn );
ZFTOOLS_API bool DXFStart( FILE* fn );
ZFTOOLS_API void DXFAddLayer(const char* layername, int col);
ZFTOOLS_API bool DXFAddDataToLayer(const char* layername, std::vector<ZFVec3dDxf>& lst);
ZFTOOLS_API void DXFBeginData();
ZFTOOLS_API void DXFEnde(); // ende sequence
ZFTOOLS_API void DXFClose(); // close file
ZFTOOLS_API void DXFDestroy();

ZFTOOLS_API void DXFAdd( ZFVec3dDxf& v, int typ, const char* layer );
ZFTOOLS_API void DXFAdd( const char* str );
// ZFTOOLS_API void DXFWriteLn( std::vector<ZFVec3d>& lst );

enum { DXFEXP_OFF, DXFEXP_PT, DXFEXP_LN };



}; // end namespace zftools

#endif //DXF_EXPORT_H
