#pragma once

#include "../filterapi.h"

#include <string>

#include <zfs/linebyline_smart.h>
#include <zfs/Tools/progress.h>

namespace zfsfilter
{
    namespace repair
    {

        struct ModifyCouterParams
        {
            int64_t add_cnt1{};
            int64_t add_cnt2{};
        };


        FILTERDLL_API std::wstring modifyCounter(std::shared_ptr<ZFSLinebyline> lbl, const ModifyCouterParams& pr, zf::cancel_progress_f prog = {} );
    }
}