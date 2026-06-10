#pragma once

#include <zfsfilter/filterapi.h>

#include <vector>
#include <string>

#include <zfs/layer_impl.h>

#include "segmentation.h"

namespace zfsfilter
{
    namespace algo
    {
        namespace filemgnt
        {
            //---------------------------------------------------------------------------------------------------------------------------
            FILTERDLL_API bool layer_type_exists_multiple_times(const std::wstring& scanFullFileName, const std::wstring& path, const std::wstring& layer_type);
            //---------------------------------------------------------------------------------------------------------------------------
            FILTERDLL_API int rename_to_base_names(const std::wstring& scanFullFileName, const std::wstring& path, const std::vector<std::wstring>& LayerIDList);
            //---------------------------------------------------------------------------------------------------------------------------
            FILTERDLL_API int delete_all_but_not(const std::wstring& scanFullFileName, const std::wstring& path, const std::vector<std::wstring>& LayerIDList);
            //---------------------------------------------------------------------------------------------------------------------------
            FILTERDLL_API int delete_png_files(const std::wstring& scanFullFileName, const std::wstring& path);
            //---------------------------------------------------------------------------------------------------------------------------
            // ===================================================================================
            FILTERDLL_API const std::vector<std::wstring> getSegmentationFileEndingsList();
            // ===================================================================================
            FILTERDLL_API const std::vector<std::wstring> get_base_layer_list();
            // ===================================================================================
            enum clean_up_option_t { KEEP_ONLY_BASE_LAYER, RENAME_LAYER_TO_BASE_NAMES };
            // ===================================================================================
            FILTERDLL_API void clean_up_layer_directory(std::wstring a, clean_up_option_t clean_up_option);
            // ===================================================================================
            FILTERDLL_API void clean_up_segmentation_files(const std::wstring& scanFullFileName, const std::wstring& path);
            // ===================================================================================
        }
    }
}