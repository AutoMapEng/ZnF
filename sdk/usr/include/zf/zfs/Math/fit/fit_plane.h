#pragma once

#include "../../zfsapi.h"
#include <vector>
#include <memory>
#include "../plane.h"

namespace zfs {
    namespace math {
        // ==========================================================================================================
        enum class plane_fit_type_t: uint32_t {standard, outlier_removal};
        // ==========================================================================================================
        /**
        \brief Result of a plane fit
        */
        class ZFS_API PlaneFit
        {
        protected:

            plane_fit_type_t plane_fit_type{ plane_fit_type_t::standard };

        public:
            
            Plane plane;
            bool succ{};  ///< true if a plane could be fitted
            double std{}; ///< std divation of points
            int points{}; ///< number of input points
            int pointsUsed{}; ///< number of points used
            int id{}; ///< user id. not used by planefit. for general usage
            std::vector<double> residuals;

            PlaneFit() = default;

            void set_plane_fit_type(const plane_fit_type_t& plane_fit_type_) 
            {
                plane_fit_type = plane_fit_type_;
            }

            plane_fit_type_t get_plane_fit_type() const 
            {
                return plane_fit_type;
            }
        };
        // ==========================================================================================================
        using PlaneFit_ptr_t = std::shared_ptr<PlaneFit>;
        // ==========================================================================================================
        /// fit plane	
        PlaneFit ZFS_API fit_plane(const std::vector<Vec3d>& points, bool ransac = true, bool residuals= false);
        // ==========================================================================================================
        /**
        \brief Result of a plane fit
        */
        class ZFS_API PlaneFitWithOutlierRemoval: public PlaneFit
        {
        public:
            std::vector<int> pointsUsedIdxVector; ///< indices of points used
            double std_to_remaining_samples{}; ///< std. to remaining samples
            std::vector<double> residuals_to_remaining_samples; ///< residuals to remaining samples

            PlaneFitWithOutlierRemoval() { this->set_plane_fit_type(plane_fit_type_t::outlier_removal); }
            PlaneFitWithOutlierRemoval(const PlaneFit& planefit) : PlaneFit(planefit) { this->set_plane_fit_type(plane_fit_type_t::outlier_removal); }
        };
        // ==========================================================================================================
        // fit plane with outlier removal given the quantil of residues that remove the outliers from the valids in the second fit, which is returned
        PlaneFitWithOutlierRemoval ZFS_API fit_plane_outlier_removal(const std::vector<Vec3d>& points, int quantil, bool residuals= false);
        // ==========================================================================================================
        using PlaneFitWithOutlierRemoval_ptr_t = std::shared_ptr<PlaneFitWithOutlierRemoval>;
        // ==========================================================================================================
    } // end namespace math
} // end namespace zfs