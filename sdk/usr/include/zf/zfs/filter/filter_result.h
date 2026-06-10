#pragma once
#include "../zfsapi.h"
#include <cstdint>
#include <map>
#include <memory>
#include <string>


namespace zfs
{
    class FilterParams;



    class FilterResult
    {
    public:
        int error{-1};                  ///< error if failed
        uint64_t pixelInDataset{};      ///< all the pixel in the dataset
        uint64_t pixelChecked{};        ///< just the valid pixel. Pixel which are already filtered out are not checked
        uint64_t pixelFilteredOut{};    ///< the number of pixel this filter has removed

        /// percent of all pixels this filter has filtered out
        double percentFilteredOut() const 
        {
            return pixelInDataset ? (double)pixelFilteredOut * 100. / pixelInDataset : 0; 
        }
        double percentChecked() const
        {
            return pixelInDataset ? (double)pixelChecked * 100. / pixelInDataset : 0;
        }

        FilterResult() = default;
        FilterResult(int err) : error(err) {}
    };



    /**
    \brief Filter result of filtering a single line
    */
    class ZFS_API LineFilterResult
    {
    public:
        bool succ{ false };
        int nPixel{ 0 };         ///< number of pixel in line / processed
        int nCorrected{ 0 };     ///< repaired by nonius logic
        int nMinRf{ 0 };         ///< pixel below min intensity
        int nFilteredOut{ 0 };   ///< filtered out

        std::map<int, int> detailed; ///< number of pixel filtered out for each filter ID first=ID second=count

        LineFilterResult() = default;

        LineFilterResult& operator += (const LineFilterResult& add)
        {
            nPixel += add.nPixel;
            nCorrected += add.nCorrected;
            nMinRf += add.nMinRf;
            nFilteredOut += add.nFilteredOut;
            for (const auto& m : add.detailed)
            {
                const auto it = detailed.find(m.first);
                if (it != detailed.end())
                    detailed[m.first] = it->second + m.second;
                else
                    detailed[m.first] = m.second;
            }
            return *this;
        }

        std::wstring resultAsString_short( bool htmColor=false) const;
    };


    /// report of the whole scan
    class ScanFilterReport
    {
    public:
        enum err_t : int
        {
            ERR_UNDEF = -1,
            ERR_OK = 0,
            ERR_NOPARAMS,
            ERR_OPENSCAN,
            ERR_WRITEMASK,
            ERR_ADDMASK,
            ERR_CALL,
        };

        std::wstring zfsFileName;   ///< the scan was filtered
        err_t errorcode{ ERR_UNDEF };    ///< errorcode
        std::shared_ptr<FilterParams> paramsUsed; ///< the paras was used
        LineFilterResult resultWholeScan; ///< detailed list of what was filtered and how many points


        ScanFilterReport(err_t e) : errorcode(e) {}
        ScanFilterReport() = default;
    };



    ZFS_API std::wstring filterCreateReport(const ScanFilterReport& report);

} // end namespace
