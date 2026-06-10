#pragma once

#include <zfsfilter/filterapi.h>

#include <string>
#include <cstdint>

#include <zfs/Math/zfvec.h>

#include <zfsfilter/tools/scan_parameter.h>
#include <zfsfilter/tools/scan_data_layer_path_generator.h>
#include <zfsfilter/tools/scan_measurement_data_layers.h>

namespace zfsfilter
{
	namespace planes
	{
		// ============================================================================================

		class plane_normal_difference_generator_t
		{
		public:
			plane_normal_difference_generator_t() = default;

			plane_normal_difference_generator_t(const std::wstring& scanFn, const scan_section_identifier_list_t& scan_section_identifier_list);

			void set(const std::wstring& scanFn, const scan_section_identifier_list_t& scan_section_identifier_list);

			void difference_angles(
				zfs::math::Vec3d normal1,
				zfs::math::Vec3d plane_tangent_x1,
				zfs::math::Vec3d plane_tangent_y1,
				zfs::math::Vec3d center_position1,
				zfs::math::Vec3d normal2,
				zfs::math::Vec3d plane_tangent_x2,
				zfs::math::Vec3d plane_tangent_y2,
				zfs::math::Vec3d center_position2,
				double& da_n, double& da_ptx, double& da_pty);

			double difference_angle_vectors(const zfs::math::Vec3d& v1, const zfs::math::Vec3d& v2);

		private:

			static const std::string normal_angle_difference_name;
			static const std::string plane_tangent_x_angle_difference_name;
			static const std::string plane_tangent_y_angle_difference_name;

			uint32_t w{ 0 }, h{ 0 };

			scan_parameter_t			             scan_parameter;
			scan_data_layer_path_generator_t         scan_data_layer_path_generator;
			measurement_data_layers_t                combined_normal_data_layers;
			scan_measurement_section_data_layers_t   scan_section_measurement_data_layers;
		};
		// ============================================================================================
	}
}