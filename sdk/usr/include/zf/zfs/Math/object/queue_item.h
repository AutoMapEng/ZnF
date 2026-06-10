#pragma once

#include <zfs/zfsapi.h>

namespace zfs {
    namespace math {
        // ======================================================================================================================
        class queue_item
        {
        public:
            void set_in_queue() { in_queue = true; }
            void unset_in_queue() { in_queue = false; }
            const bool& is_in_queue() const { return in_queue; }
        private:
            bool in_queue{ false };
        };
        // ======================================================================================================================
    } // end namespace math
} // end namespace zfs