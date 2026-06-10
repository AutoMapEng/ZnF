#pragma once

#include <zfsfilter/filterapi.h>

#include <string>


namespace zfsfilter
{
    namespace algo
    {
        #define LAYER_NAME_GLAS_CANDIDATES L"glas"

        /**
        \brief lookup for candidates, generates all layers and combines this
            generate several output layers
            !!! This is just a expermimental code !!!
        */
        FILTERDLL_API std::string glassCandidates(const std::wstring& a, const std::wstring& param=L"ns:v3.0.005;nd:v3.0.001", bool force=false, bool createImages=false, bool rotateImages=true);

        // ** generate specific target layer (and all layers necessary for the generation)
        FILTERDLL_API void generateSegmentationLayer(const std::wstring& scan, bool force, bool genpng);
        FILTERDLL_API void generateRegistrationMask(const std::wstring& scan, bool force, bool genpng, const std::wstring& min_object_size, const std::wstring& max_no_of_segments);
    }
}