#pragma once
#include "zfcore_api.h"

#include <string>

namespace zf
{
	ZFCORE_API std::string utf16_to_utf8(const std::wstring& ws);

	ZFCORE_API std::wstring utf8_to_utf16(const std::string& ws);



	ZFCORE_API std::wstring cp_to_utf16(const std::string& szText);

	ZFCORE_API std::string utf16_to_cp(const std::wstring& szText);


	ZFCORE_API std::string cp_to_utf8(const std::string& szText);

	ZFCORE_API std::string utf8_to_cp(const std::string& szText);


	// obsolete
	ZFCORE_API
	std::string& UTF16_to_CP(const wchar_t* szText, std::string& resultString);

}
