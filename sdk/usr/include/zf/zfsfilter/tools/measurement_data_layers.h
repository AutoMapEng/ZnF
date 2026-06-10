#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>
#include <string>
#include <utility>
#include <map>
#include <set>
#include <list>
#include <memory>
#include <vector>

#include <zfs/layer.h>
#include <zfs/layer_impl.h>

namespace zfsfilter
{

    // ======================================================================================================================

    using layer_type_t = zfs::ZFLayer::typ_t;
    using layer_specifier_t = std::pair<std::string, layer_type_t>;

    using layer_specifier_set_t = std::set<layer_specifier_t>;
    using file_name_map_t = std::map<std::string, std::wstring>;
    using layer_ptr_t = std::shared_ptr<zfs::ZFLayer>;

    // ======================================================================================================================

    struct FILTERDLL_API measurement_data_layer_t
    {
        static std::wstring layer_file_name(const std::wstring& layer_file_path_, const std::wstring& layer_name_prefix_, const std::string layer_name_);

        measurement_data_layer_t() = default;

        measurement_data_layer_t(const std::wstring& layer_path_, const layer_type_t& layer_type_);

        void set_type(const layer_type_t& layer_type_);

        void set_path(const std::wstring& layer_path_);

        bool alloc_layer();

        bool create(const uint32_t& w, const uint32_t& h);

        bool open(uint32_t& w_, uint32_t& h_);

        void close();

        layer_ptr_t get();


    private:
        bool alloc_OK{ false };
        std::wstring layer_path;
        layer_type_t layer_type{ zfs::ZFLayer::typ_t::TYP_UNDEF };
        layer_ptr_t layer_ptr;
    };

    // ==========================================================================================================

    using measurement_data_layer_map_t = std::map<std::string, measurement_data_layer_t>;

    // ==========================================================================================================

    struct FILTERDLL_API measurement_data_layers_t
    {
        measurement_data_layers_t() = default;

        measurement_data_layers_t( const std::wstring& layer_file_path_,
                                   const std::wstring& layer_file_name_prefix_,
                                   const layer_specifier_set_t& layer_specifier_set_);
                                   
        void set(const std::wstring& layer_file_path_,
               const std::wstring& layer_file_name_prefix_,
               const layer_specifier_set_t& layer_specifier_set_);

        bool create_layers(const uint32_t& w_, const uint32_t& h_);

        bool read_layers(uint32_t& w_,uint32_t& h_);

        void close_layers();

        bool is_inputOK() const;

        layer_ptr_t get_layer(const std::string& layer_name);

    private:

        using layer_name_list_t = std::vector<std::string>;
        using layer_type_list_t = std::vector<layer_type_t>;

        bool                  inputOK{ false };

        size_t                number_of_layers{ 0 };

        uint32_t              w{ 0 }, h{ 0 };

        std::wstring          layer_file_path;
        std::wstring          layer_file_name_prefix;
        layer_specifier_set_t layer_specifier_set;
        layer_name_list_t     layer_name_list;
        layer_type_list_t     layer_type_list;

        file_name_map_t       layer_name_map;

        measurement_data_layer_map_t measurement_data_layer_map;

        bool generate_measurement_data_layer_map();

        void generate_layer_name_map();

        void prepare_layer_specifier_lists();

    };
    // ======================================================================================================================
}