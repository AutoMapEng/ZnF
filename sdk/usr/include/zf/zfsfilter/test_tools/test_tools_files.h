#pragma once

// shifted to zftools/folder2.h

#if 0 

#include <vector>
#include <string>

#include <zfsfilter/filterapi.h>

namespace zfsfilter
{
    namespace test
    {
        // ==============================================================================================
        FILTERDLL_API std::vector<std::wstring> listFilesWithExtentionInDirectory(const std::wstring &directory, const std::wstring& extention);
        // ==============================================================================================
        FILTERDLL_API std::vector<std::wstring> listSubDirectories(const std::wstring &directory);
        // ==============================================================================================

    }
}

#endif