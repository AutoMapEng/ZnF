#pragma once

#include <zfsfilter/filterapi.h>


#include <string>
#include <vector>
#include <utility>


#ifdef _WINDOWS
namespace zfsfilter
{
    namespace repair
    {
        /**
            \brief cut links if one part is missing. So the rest can be read normaly
            \return the list of the parts
        */
        FILTERDLL_API std::vector< std::pair<int, std::wstring> > repairLinked( const std::wstring& absZfsFileNamePart0 );


        /**
        \brief rebuild linked list if it does not exist.
               Filenames must match <scanname>.part<nr>.zfs
               Searches for .part and create a new linked list
        \exception std::exception on failure
        */
        FILTERDLL_API std::vector< std::pair<int, std::wstring> > buildLinkedListX(const std::wstring& first);

    }
}
#endif // _WINDOWS