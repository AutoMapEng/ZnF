#ifndef __ZFPROJECT_CLEANUP_H__
#define __ZFPROJECT_CLEANUP_H__

#include "ZfsPrjBasic.h"
#include "config.h"

class ZFProject;

namespace zfprj
{

/**
\brief Moves files into the destination folder
             *.log will be moved to .temp
\return number of files moved
*/
ZFPRJ_API int Prj_Cleanup( ZFProject& p );

} // end namespace

#endif //__ZFPROJECT_CLEANUP_H__
