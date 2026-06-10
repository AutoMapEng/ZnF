#pragma once

#include <string>
#include <zfsfilter/filterapi.h>

class ZFSLinebyline;
class ZFLineByLineWrite;

namespace zfsfilter {

    /**
     * @brief Repair Horizontal Encoder jumps on 315 scans
     *
     * Scans created with I5016 with Firmware below or equal to SVN 6293 may show
     * jumps on horizontal encoder around 0 position. This function trys to
     * fix them. If lines can't get fixed, broken lines are removed.
     *
     * Before SVN 6418, there also a possible bug in writing the ZFS-Header 69,
     * which is also corrected.
     *
     * @param src Source scan to repair
     * @param out Target to write to
     * @return number of changed lines
     */
    FILTERDLL_API int repair_315_horzjumps(
            ZFSLinebyline& src, ZFLineByLineWrite& out);

    /**
     * @brief Repair scan, addressed by file name
     *
     * Output file is removed again, if no line has been changed.
     *
     * @param scan Scan to repair
     * @param out  File to write corrected scan to. If empty, gets written to
     *             subfolder "repaired/" using the same file name
     * @return number of changed lines
     */
    FILTERDLL_API int repair_315_horzjumps( 
            const std::wstring& scan, const std::wstring& out=L"");

    /**
    * @brief Repair scan, 69=68-67 if not recalculate
    *
    * @return true when was repaired
    */
    FILTERDLL_API bool repair_69( const std::wstring& scan );

} // namespace zfsfilter
