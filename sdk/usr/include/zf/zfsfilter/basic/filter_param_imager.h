#pragma once
#include <zfsfilter/filterapi.h>

#include <zfs/filter/filter_param.h>


namespace zfsfilter
{

    class FILTERDLL_API ImagerFilterParams : public zfs::FilterParams
    {
    public:
        ImagerFilterParams() = default;

//        virtual void extendDefinitions(const zfs::FilterSetupParams&) override;
    };


} // namespace
