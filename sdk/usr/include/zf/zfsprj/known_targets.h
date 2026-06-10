#pragma once

#include "ZfsPrjBasic.h"

#include "target.h"

#include <string>

class ZFProject;


namespace zfprj
{
    /// return number of known targets in project
    ZFSPRJ_API int numOfKnownTargetsInProject(ZFProject& p);

    /**
    \brief add known targets to the project
    \return number of targets added
    */
    ZFSPRJ_API int addKnownTargetsToProject(ZFProject& p, const targetlist_t& );

    /**
    *   Add known targets from file
    *   return number of targets added
    */
    ZFSPRJ_API int addKnownTargetsToProject( ZFProject& proj, const std::wstring& file );
}

