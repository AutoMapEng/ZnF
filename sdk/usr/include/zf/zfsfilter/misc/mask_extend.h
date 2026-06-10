#pragma once
#include "../filterapi.h"
#include <string>
#include <memory>

class ZFMask;
class ZFSLinebyline;

namespace zfsfilter
{

    /*
    * \brief extend the mask by param extend. Set or clear depending on inverted
    * \return the number of pixel on which the mask was extended
    */
    FILTERDLL_API
    int mask_extend_viamask(ZFMask* src, ZFMask* dest, int extend, bool inverted=false);

    FILTERDLL_API
    int mask_extend_vialbl(std::shared_ptr<ZFSLinebyline> lbl, int extend, const std::wstring& mask2Extend, const std::wstring& maskExtended, bool inverted=false);

    FILTERDLL_API
    int mask_extend_viafilename(const std::wstring& zfs, int extend, const std::wstring& mask2Extend, const std::wstring& maskExtended, bool inverted=false );

} // end namespace

