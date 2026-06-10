#pragma once
#include "zftools_api.h"
#include <zfcore/compiler.h>

#include <string>



class ZFTOOLS_API FilePath
{
	static std::wstring sUserAppPath;
    static std::wstring wsAppDataPath;

public:
	const wchar_t* GetUserAppDataPath();
    const wchar_t* GetAppPath();
};

