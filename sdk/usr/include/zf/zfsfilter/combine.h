#pragma once
/**
*	Bewegliche Objekte aus zwei Scans loeschen.
*	Scans muessen genau am gleichen Standpunkt aufgenommen worden sein.
*/
#include "filterapi.h"
#include "combine_def.h"

#include <string>
#include <vector>
#include <memory>

#include "zfsfilter_exception.h"
#include <zfs/Tools/progress.h>


class ZFScan;
class ZFSLinebyline;
class ZFLineByLineWrite;

namespace zfs
{
    class ProgressCallbackInterface;
    typedef std::shared_ptr<ZFSLinebyline> zfslinebyline_ptr;
}

namespace zfsfilter
{
    class CombineError : public ZFSFilterException {
    public:

        /**
         * @brief Construct a CombineError
         *
         * @param code Reason of the error
         * @param msg Additional textual information
         */
        CombineError(combine_err_t code, const std::string& msg)
            : ZFSFilterException(msg)
            , m_code(code)
        { }

        /**
         * @brief Return the error code
         */
        combine_err_t code() const
        {
            return m_code;
        }

    private:
        combine_err_t m_code; ///< Error code; reason of the exception
    };

    /**
     * @brief Create a combined scan from master and slaves.
     *
     * In the slave scans is looked for positions from master. If those points
     * within one of the slaves is better (higher range) the point from one
     * of those slaves is taken instead the masters point.
     *
     * The result is written into @c out.
     *
     * All scans should be taken at the same position (Header entry 590), but
     * this function doesn't enforce it.
     * At least one slave scan must be provided.
     *
     * Slave scan must be opened with the Open-Flag OPEN_XYZ2POS set. Else no
     * backtranformation can be performed.
     *
     * @param master Master (or reference) scan
     * @param scans  Slave scans
     * @param out    Result is written here
     * @param param  Additional parameters for controlling fusion.
     * @param progress Callback to provide the information about
     *                 progress and the ability to abort combining
     */
    void FILTERDLL_API CombineScans(
            zfs::zfslinebyline_ptr master,
            std::vector<zfs::zfslinebyline_ptr> slaves,
            ZFLineByLineWrite& out,
            const Combine2ScansParam& param,
            Combine2ScansResult& result,
            zfs::ProgressCallbackInterface* progress=nullptr,
            zf::progress_f prog = nullptr);


    /**
     * @brief Combine scans and replace master file
     *
     * Opens all the scans, specified by file names, merges them and
     * copies the result back over the master.
     *
     * Behind the scene CombineScansd() is called. So the same
     * limitations apply.
     *
     * @note On success the original scan file is overridden!
     *
     * @param master Path to master scan
     * @param slaves List of slave scans to merge into master
     * @param param  Additional paramters for controlling fusion
     * @param [out] result Result structure
     * @param progress Callback to provide the information about
     *                 progress and the ability to abort combining
     */
    void FILTERDLL_API CombineScanFiles(
            const std::wstring& master,
            const std::vector<std::wstring>& slaves,
            const Combine2ScansParam& param,
            Combine2ScansResult& result,
            zfs::ProgressCallbackInterface* progress=nullptr, zf::progress_f prog= nullptr);

} // end namespace
