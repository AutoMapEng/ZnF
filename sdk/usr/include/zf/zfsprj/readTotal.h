#pragma once
#include "ZfsPrjBasic.h"

#include <string>

#include "vecname.h"
#include "target.h"

class ZFUnit;

//===============================================================================================

/**
\brief read targets from total station file
       adds the targets to the list
       *.K *.IDX and userdefined *.TXT are possible
\return number of targets read. This is not the number of targets in the list because there already targets in the list
*/
ZFSPRJ_API int readTotal(const std::wstring& filename, targetlist_t& lst, std::wstring& error, const ZFUnit* unit = {} );

/**
\brief read tragets from a stringlist format: name x y z
*/
ZFSPRJ_API targetlist_t readTotalFromStringList(const std::vector< std::string>& str);

//TARGET_API int readTotal(const wchar_t* filename, vecnamelst_t& lst, std::wstring& error, bool lowercase = false);

ZFSPRJ_API VecName* findTotal(vecnamelst_t& lst, const char* name);

/// supported file extensions for reading total station files
ZFSPRJ_API std::string readTotal_SupportedFileExtensions();

/// VecName_Common returns the number of identical target-names in the two vecnamlists
ZFSPRJ_API int VecName_Common(const vecnamelst_t&, const vecnamelst_t&);

/// suche ob name in der liste
ZFSPRJ_API bool VecName_Has(const vecnamelst_t&, const wchar_t*);

/**
\brief zwei listen zusammenfuehren
       Ergebniss ist in Liste eins
\return anzahl neu hinzugefuegter Eintraege
*/
//TARGET_API int VecName_Combine(targetlist_t& list, const targetlist_t& listAdd, bool casesens = false);

/**
\brief Zwei listen zusammenfuehren. Nur die Targets die benutzt werden
*/
ZFSPRJ_API int Combine(vecnamelst_t& dest, const vecnamelst_t& add, bool use);
ZFSPRJ_API int Combine(targetlist_t& dest, const targetlist_t& add, bool use);
ZFSPRJ_API int Combine(targetlist_t& dest, const vecnamelst_t& add, bool use);


