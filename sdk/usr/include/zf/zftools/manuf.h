#pragma once

#include "zftools_api.h"

#include <string>

namespace zftools
{
	enum manuf_t { MANUF_UNDEFINE, MANUF_ZF, MANUF_API, MANUF_PENTAX, MANUF_STONEX, MANUF_LEICA };
	ZFTOOLS_API bool ManufDefine( manuf_t );
	ZFTOOLS_API manuf_t ManufGet();

    ZFTOOLS_API void ManufSetAppVersion( const std::wstring& v );

	ZFTOOLS_API std::wstring ManufSplashScreen(bool scout=false);
	ZFTOOLS_API std::wstring ManufCompany();
	ZFTOOLS_API std::wstring ManufCompanyShort();
	ZFTOOLS_API std::wstring ManufAppName();
	ZFTOOLS_API std::wstring ManufAppVersion();
	ZFTOOLS_API std::wstring ManufAppIcon();
	ZFTOOLS_API std::wstring ManufHomepage();
	ZFTOOLS_API std::wstring ManufSupport();
	ZFTOOLS_API std::wstring ManufSoftwareUpdate();
	ZFTOOLS_API std::wstring ManufHelp( const std::wstring& company, const std::wstring& lang, bool pdf );

	ZFTOOLS_API std::wstring ManufAppData();

} // end namespace
