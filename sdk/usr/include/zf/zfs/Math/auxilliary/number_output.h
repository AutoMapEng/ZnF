#pragma once

#include <cstdint>
#include <iostream>

namespace zfs {
    namespace math {
        // uint8_t, int8_t and char need special handling, if they should get
        // read/written as integer numbers!
        std::ostream& operator<< (std::ostream& ofs, uint8_t v);
        std::istream& operator >> (std::istream& ifs, uint8_t& v);
        std::ostream& operator<< (std::ostream& ofs, int8_t v);
        std::istream& operator >> (std::istream& ifs, int8_t& v);
    } // end namespace math
} // end namespace zfs