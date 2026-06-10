#pragma once

#include <exception>
#include <vector>
#include <string>

#include <zfs/zfsapi.h>
#include <zfs/zfi/zfi_mmap.h>

namespace zfs {
namespace zfi {

class CombineError : public std::exception
{
public:
    CombineError(const std::string& msg)
        : m_msg(msg) { }

private:
    const char* what() const throw()
    {
        return m_msg.c_str();
    }

    std::string m_msg; ///< message, describing the error
};

/**
 * @brief Combine two or more ZFI files into one single file
 *
 * All files from master scan are copied into the output file specified by @c
 * out as they are (however IDs and names may change). From the other files,
 * all image (series) are copied, but marked as REMOVED. This way the REMOVED
 * images can later get marked as non REMOVED and used for colorizing scans.
 *
 * Due to constaints in LaserControl, the image names must be equal the scan
 * and ZFI base name. So the file names in the output file are adjusted
 * accondingly to match @c out's base name, if parameter @c name is unset.
 *
 * The IDs (first number after '{') from other ZFI files are ajusted to fit into
 * the ordering of the master file. However in future also the master files IDs
 * may change (in case additional WB or BB pictures are added)
 *
 * It's the callers responsibility to make sure combination of ZFI makes sense:
 * e.g. I-Cam container vs. T-Cam container
 *
 * @param master Master ZFI file
 * @param other  Other files whose image series are to be added to the masters
 *               one (may also be empty)
 * @param out    Output file to write. Must not exist!
 * @param name   name to use for pic series (part before '{'). If empty,
 *               generate name from @c out by removing extension and path.
 *
 * @throw CombineError in case of errors
 */
void ZFS_API combine(
        const Zfi& master,
        const std::vector<Zfi>& other,
        const std::wstring& out,
        std::wstring name=L"");

} // namespace zfi
} // namespace zfs
