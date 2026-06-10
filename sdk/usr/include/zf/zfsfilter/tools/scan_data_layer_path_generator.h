#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>
#include <string>
#include <vector>

#include "../tools/scan_parameter.h"

namespace zfsfilter
{
	// ============================================================================================

	using vec_ws = std::vector<std::wstring>;

	// ============================================================================================

	using scan_section_identifier_t = uint32_t;
	using scan_section_identifier_list_t = std::vector<scan_section_identifier_t>;

	// ============================================================================================

	class FILTERDLL_API scan_data_layer_path_generator_t
	{
	public:
    
		scan_data_layer_path_generator_t() = default;
		scan_data_layer_path_generator_t(const std::wstring& scanFn, scan_section_identifier_t scan_section_identifier);
		scan_data_layer_path_generator_t(const std::wstring& scanFn, const scan_section_identifier_list_t& scan_section_identifier_list);

		bool set(const std::wstring& scanFn_, scan_section_identifier_t scan_section_identifier);
		bool set(const std::wstring& scanFn_, const scan_section_identifier_list_t& scan_section_identifier_list);

		uint64_t get_number_of_scan_sections() const; // < number of full sections of scan
		const std::wstring& get_scan_name() const; // < scan name
		const std::wstring& get_scan_path() const; // < path of scan
		const std::wstring& get_default_path() const; // < path to section layer main directory

		std::wstring get_data_layer_file_path_and_name_prefix(const uint32_t& scan_section_idx= 0) const; // < prefix for data layers in section directory

		const std::wstring& get_scan_sections_data_layer_path() const; // < path to section layer main directory
		const std::wstring& get_scan_section_path(const uint32_t& scan_section_idx = 0) const; // < path to section layer directory with idx scan_section_idx
		const std::wstring& get_parameter_test_path(const uint32_t& scan_section_idx = 0) const; // < path to parameter test directory of section layer directory with idx scan_section_idx
		const std::wstring& get_plane_connector_test_path(const uint32_t& scan_section_idx = 0) const; // < path to plane connector test directory of section layer directory with idx scan_section_idx

		const vec_ws& get_scan_section_paths() const; // < vector/list of scan section directories
		const vec_ws& get_parameter_test_paths() const; // < vector/list of parameter test paths for every scan section directory
		const vec_ws& get_plane_connector_test_paths() const; // < vector/list of plane connector test paths for every scan section directory


	private:

		std::wstring parameter_test_path_dir_name{ L"parameter_tests" };
		std::wstring plane_connector_test_path_dir_name{ L"plane_connector_tests" };

		bool inputOK{ false };

		uint32_t number_of_scan_sections{ 0 };

		std::wstring scan_file_path;
		std::wstring scan_path;
		std::wstring scan_name;

		std::wstring scan_sections_data_layer_directory;

		scan_parameter_t scan_parameter;

		vec_ws section_paths;
		vec_ws parameter_test_paths;
		vec_ws plane_connector_test_paths;
	};

	// ============================================================================================
}