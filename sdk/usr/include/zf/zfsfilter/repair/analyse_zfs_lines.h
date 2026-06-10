#pragma once

//#define STRAYLIGHT_HISTO

#include <zfsfilter/filterapi.h>

#include <string>
#include <utility>
#include <functional>

#ifdef _WINDOWS
namespace zfsfilter
{
    namespace repair
    {

        /**
        \brief checks wheather the file is corrupted and if it can be repaired
        */
        class AnalyseZFS4Repair
        {
        public:
            bool scanOk{ false }; ///< nothing further to do
            bool repairPossible{ false };

            int linesFound{ 0 };
        };
        typedef std::function< void(const std::string&) > info_cb_f;
        FILTERDLL_API AnalyseZFS4Repair analyse_zfslines( const std::wstring& fn, info_cb_f cb=nullptr);

    }
}
#endif
