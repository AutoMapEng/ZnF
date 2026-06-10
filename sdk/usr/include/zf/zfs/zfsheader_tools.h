#pragma once

#include "zfsapi.h"

#include <string>
#include <vector>
#include <map>

class ZFSHeader;

namespace zfs
{
	/**
	\brief compares two zfs header and return the differences as a string
	*/
	std::string ZFS_API compareZfsHeader(const ZFSHeader& one, const ZFSHeader& two);

    /// parse string and put items to zfsheader. stringformat: key=value
    ZFS_API int string2zfsheader(const std::string& str, ZFSHeader& header, const char separator='\n');

    /// already a list of strings like key=value
    ZFS_API int string2zfsheader(const std::vector< std::string >& lst, ZFSHeader& header);


    /// return a xyl string of the zfs-header
    ZFS_API std::string zfsheader2xml(const ZFSHeader& header);


    /**
    @brief read the zfs.ini file and return as a map
    */

    /// Description of a single zfs.ini item
    class ZFS_API ZfsIniItem
    {
    public:
        enum type_t { UNDEF, INT, DOUBLE, FLOAT2, STRING };
        type_t typ = UNDEF; ///< the type of the item
        std::wstring desc;   ///< description text
        std::wstring unit;   ///< optional a unit

        ZfsIniItem() {}
        ZfsIniItem(type_t t, const std::wstring& d, const std::wstring& u = L"") : typ(t), desc(d), unit(u) {}
        
        /// return the type as a text understandable for the enduser 
        static std::wstring typeAsString(type_t);
    };
    typedef std::map<int, ZfsIniItem> ZfsIniMap; 
    ZFS_API ZfsIniMap parse_zfsini(const std::wstring& inifile);

}