#pragma once

#include <zfcore/arg.h>

namespace zftools
{
    //TODO interface auf zf::Argument umstellen und dann folgende zeile entfernen
    using namespace zf;
}

#if 0
#include "zftools_api.h"

#include <string>
#include <vector>
#include <map> 


namespace zftools
{
    /**
    \brief Arguments from command line or ini file
    */
    class ZFTOOLS_API Argument
    {
    public:
        Argument() {}
        Argument(int argc, char* argv[]);
        Argument(const std::string& ); // use parseAndAddItems

        typedef std::map<std::string, std::string> arg_map;

        /// remove the item from the list
        void removeItem(const std::string& item);

        /// add item and set to true
        void addItem(const std::string& item);

        /// add a argument to the list
        void addItem(const std::string& item, const std::string& value);
        void addItem(const std::string& item, const std::wstring& value);

        /// parse a single param in the format like: rotate=-30
        void parseAndAddItem(const std::string&);

        /// parse several params in the format like: -rotate 50 -translate 30
        void parseAndAddItems(const std::string&);
        void parseAndAddItems(const std::vector< std::string >& lst);

        /// read the arguments from a ini file
        bool parseFile(const std::wstring& fn);
        bool parseFile(const std::string& fn);

        /// check wheater argument exists
        bool has(const std::string& item) const;

        // check wheater argument has a value
        bool hasValue(const std::string& item) const;

        /// get value as string
        std::string getString(const std::string& item, const std::string& ="") const;
        std::wstring getWString(const std::string& item, const std::wstring& = L"") const;

        /// get value as int
        int getInt(const std::string& item, int def=0) const;

        /// get value as int
        bool getBool(const std::string& item, bool def = 0) const;

        /// get value as double
        double getDouble(const std::string& item, double def=0.) const;

        /// return number of items
        int count() const;

        /// create a string containing all params and values
        std::string buildString() const;

        /// clear everything
        void clear();

    protected:
        arg_map args; ///< the arguments
    };
}
#endif