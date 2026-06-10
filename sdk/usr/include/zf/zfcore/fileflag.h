#pragma once
#include "zfcore_api.h"


#include <string>
#include <vector>


namespace zf
{
    #define ext_zf_flags   ".flag"
    #define wext_zf_flags   L".flag"


    /**
     * Will create/remove a flag file in subfolder .temp
     * ./.temp/<fileName>.<flagname>.flag
     */
    ZFCORE_API void setFileFlag_w(const std::wstring& fileName, const std::wstring& flagname, bool dirty = true);
    ZFCORE_API void setFileFlag(const std::wstring& fileName, const std::string& flagname, bool dirty = true);

    ZFCORE_API bool isFileFlagSet_w(const std::wstring& fileName, const std::wstring& flagname);
    ZFCORE_API bool isFileFlagSet(const std::wstring& fileName, const std::string& flagname);

    /**
     * \return files deleted.
     */
    ZFCORE_API int removeAllFlagFiles(const std::wstring& fileName );

    ZFCORE_API void removeFlagFiles_w(const std::wstring& fileName, const std::vector<std::wstring>& flagnames );
    ZFCORE_API void removeFlagFiles(const std::wstring& fileName, const std::vector<std::string>& flagnames);

}
