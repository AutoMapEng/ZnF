#pragma once

#include <list>

namespace zfs {
    namespace math {
        //################################################################################
        // used data type abbreviations
        typedef std::list<bool>           BoolListType;
        typedef std::list<int>            IntListType;
        typedef std::list<std::list<int>> IntListListType;
        typedef std::list<double>         DoubleListType;
    } // end namespace math
} // end namespace zfs