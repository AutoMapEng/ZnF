/**
\brief Allow acces to text based ini files
*/

#pragma once

#include "zftools_api.h"

#include <string>
#include <map> 

namespace zftools
{


    //#########################################################################################################

    typedef std::map<std::string, std::wstring> m1;
    typedef std::map<std::string, m1> m2;



    /**
    \brief Class that stores and loads key-value pairs to/from an ini file.
    */
    class ZFTOOLS_API IniFile
    {
    public:
        IniFile() {}
        virtual ~IniFile() {}

        bool Read(const std::wstring&);
        bool Reload();

        bool Write(const std::wstring&);

        std::wstring FileName() const { return filename; }


        double GetDouble(const char* sek, const char* item, double def = 0.) const;
        int GetInt(const char* sek, const char* item, int def = 0) const;
        std::wstring GetStr(const char* sek, const char* item, const std::wstring& def = L"") const;
        bool GetBool(const char* sek, const char* item, bool def = false) const;

        void SetStr(const char* sek, const char* item, const std::wstring& str);
        void SetInt(const char* sek, const char* item, int);
        void SetDouble(const char* sek, const char* item, double d, int n = 10);
        void SetDbl(const char* sek, const char* item, double d, int n = 10) { SetDouble(sek, item, d, n); }
        void SetBool(const char* sek, const char* item, bool val) { SetInt(sek, item, val ? 1 : 0); }

        bool Exist(const char* sek) const;						///< check for group
        bool Exist(const char* sek, const char* item) const;	///< check for item in group

        bool Remove(const char* sek);						///< remove group
        bool Remove(const char* sek, const char* item);	///< remove item in group

    private:
        m2::iterator getIteratorToSectionItemInMymap(const char* sek);
        m2::const_iterator getIteratorToSectionItemInMymap(const char* sek) const;

    private:
        std::wstring filename;	///< filename of ini file
        m2 mymap;
    };

}