#pragma once

#include <zfs/zfsapi.h>

namespace zfs {
    namespace math {
        // ======================================================================================================================
        class ZFS_API stack_item
        {
        public:
            void set_on_stack() { on_stack = true; }
            void unset_on_stack() { on_stack = false; }
            const bool& is_on_stack() const { return on_stack; }
        private:
            bool on_stack{ false };
        };
        // ======================================================================================================================
    } // end namespace math
} // end namespace zfs