#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>
#include <string>
#include <map>

#include "../tools/scan_data_layer_path_generator.h"

#include "scan_parameter.h"
#include "measurement_data_layers.h"

namespace zfsfilter
{
    // ======================================================================================================================

    struct FILTERDLL_API scan_measurement_data_layers_t : public measurement_data_layers_t
    {
        scan_measurement_data_layers_t() = default;
        scan_measurement_data_layers_t(const std::wstring& scanFn_,
            const layer_specifier_set_t& layer_specifier_set_,
            const scan_section_identifier_t& scan_section_identifier_);

        void set(const std::wstring& scanFn_,
            const layer_specifier_set_t& layer_specifier_set_,
            const scan_section_identifier_t& scan_section_identifier_);

        bool create_layers(const uint32_t& w_, const uint32_t& h_);

        bool read_layers(uint32_t& w_, uint32_t& h_);

        void close_layers();

        const scan_parameter_t& get_scan_parameter() const;

    private:

        std::wstring  scan_layer_directory;
        std::wstring  scan_layer_file_name_prefix;

        scan_parameter_t scan_parameter;
        scan_data_layer_path_generator_t scan_data_layer_path_generator;

    };
    // ======================================================================================================================

    using scan_section_measurement_layer_map_t = std::map<uint32_t, scan_measurement_data_layers_t>;

    // ======================================================================================================================

    struct FILTERDLL_API scan_measurement_section_data_layers_t
    {
        scan_measurement_section_data_layers_t() = default;
        scan_measurement_section_data_layers_t(const std::wstring& scanFn_, const layer_specifier_set_t& layer_specifier_set_, const scan_section_identifier_list_t& scan_section_identifier_list_);

        void set(const std::wstring& scanFn_, const layer_specifier_set_t& layer_specifier_set_, const scan_section_identifier_list_t& scan_section_identifier_list_);

        bool create_layers(const uint32_t& w_, const uint32_t& h_);

        bool read_layers(uint32_t& w_, uint32_t& h_);

        void close_layers();

        layer_ptr_t get_layer(const uint32_t& scan_section, const std::string& layer_name);

    private:

        scan_parameter_t scan_parameter;
        scan_section_measurement_layer_map_t scan_section_measurement_layer_map;

        std::vector<uint32_t> scan_section_identifier_list;

    };
    // ======================================================================================================================

}