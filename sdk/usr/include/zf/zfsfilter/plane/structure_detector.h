#pragma once

#include <zfsfilter/filterapi.h>

#include <zfs/Math/math/average.h>
#include <zfs/Math/signal_processing/dft2.h>

namespace zfsfilter
{
    namespace planes
    {
        // ==================================================================================

        struct FILTERDLL_API structure_detector
        {
            structure_detector() = default;
            structure_detector(const int& size, const double& sigma_multiple_, const double& min_peak_amplitude_);
            structure_detector(const int& rows, const int& columns, const double& sigma_multiple_, const double& min_peak_amplitude_);

            void initialize(const int& rows, const int& columns, const double& sigma_multiple_, const double& min_peak_amplitude_);

            bool detect(const std::vector<double>& perpendicular_surface_fit_residuals);

        private:
            double sigma_multiple{ 1. };
            double min_peak_amplitude{ 0 };
            zfs::math::signal_array_2D<double> residual_array;
            zfs::math::dft2<double> residuals_dft;

        };

        // ==================================================================================
    }
}