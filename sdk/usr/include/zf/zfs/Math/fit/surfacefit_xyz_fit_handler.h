#pragma once

#include <memory>
#include <map>

#include <zfs/Math/fit/surfacefit.h>

namespace zfs {
    namespace math {
        // ==============================================================================================
        // fit management class to enable choosing the surface fit to use (without the need to call delete in any place)
        // ==============================================================================================
        using p_surfacefit = std::shared_ptr<surface_fit>;
        using p_surfacefit_gj = std::shared_ptr<surface_fit_gj>;
        using p_surfacefit_lu = std::shared_ptr<surface_fit_lu>;
        using p_surfacefit_chol = std::shared_ptr<surface_fit_chol>;

        // ==============================================================================================
        // surface_fit_xyz-helper classes
        // implements common fits that are realized by derivation-class specific functions,
        // for which no virtual table entry exists.
        // ==============================================================================================
        struct surfacefit_xyz_common_param_fit_handler_base
        {
            surfacefit_xyz_common_param_fit_handler_base()
            {}

            virtual ~surfacefit_xyz_common_param_fit_handler_base() {};

            virtual bool fit(const vec& u, const vec& v, const vec& x, const vec& y, const vec& z, const double& sigma, const vec_b& subsample) = 0;
            virtual bool fit(const vec& u, const vec& v, const vec& x, const vec& y, const vec& z, const double& sigma_x, const double& sigma_y, const double& sigma_z, const vec_b& subsample) = 0;
            virtual bool fit(const vec& u, const vec& v, const vec& x, const vec& y, const vec& z, const vec& sigma, const vec_b& subsample) = 0;

        protected:

            bool             m_b_fit_ok{ true };
        };
        // ==============================================================================================
        struct surfacefit_xyz_common_param_fit_handler_gj : public surfacefit_xyz_common_param_fit_handler_base
        {
            surfacefit_xyz_common_param_fit_handler_gj()
            {}

            surfacefit_xyz_common_param_fit_handler_gj(
                p_surfacefit& surface_fit_x,
                p_surfacefit& surface_fit_y,
                p_surfacefit& surface_fit_z)
                : surfacefit_xyz_common_param_fit_handler_base(),
                m_surface_fit_x(std::dynamic_pointer_cast<surface_fit_gj>(surface_fit_x)), // evaluates to nullptr if not suitable
                m_surface_fit_y(std::dynamic_pointer_cast<surface_fit_gj>(surface_fit_y)),
                m_surface_fit_z(std::dynamic_pointer_cast<surface_fit_gj>(surface_fit_z))
            {}

            virtual bool fit(const vec& u, const vec& v, const vec& x, const vec& y, const vec& z, const vec& sigma, const vec_b& subsample) override
            {
                m_b_fit_ok = m_surface_fit_x->fit(u, v, x, sigma, subsample);

                const matrix<double>& dcm = m_surface_fit_x->get_dcm();
                const base_term_storage& bts = m_surface_fit_x->get_base_term_storage();

                m_b_fit_ok = m_b_fit_ok && m_surface_fit_y->fit(dcm, bts, u, v, y, sigma, subsample);
                m_b_fit_ok = m_b_fit_ok && m_surface_fit_z->fit(dcm, bts, u, v, z, sigma, subsample);

                return m_b_fit_ok;
            }

            virtual bool fit(const vec& u, const vec& v, const vec& x, const vec& y, const vec& z, const double& sigma, const vec_b& subsample) override
            {
                m_b_fit_ok = m_surface_fit_x->fit(u, v, x, sigma, subsample);

                const matrix<double>& dcm = m_surface_fit_x->get_dcm();
                const base_term_storage& bts = m_surface_fit_x->get_base_term_storage();

                m_b_fit_ok = m_b_fit_ok && m_surface_fit_y->fit(dcm, bts, u, v, y, sigma, subsample);
                m_b_fit_ok = m_b_fit_ok && m_surface_fit_z->fit(dcm, bts, u, v, z, sigma, subsample);

                return m_b_fit_ok;
            }

            virtual bool fit(const vec& u, const vec& v, const vec& x, const vec& y, const vec& z, const double& sigma_x, const double& sigma_y, const double& sigma_z, const vec_b& subsample) override
            {
                m_b_fit_ok = m_surface_fit_x->fit(u, v, x, sigma_x, subsample);

                const matrix<double>& dcm = m_surface_fit_x->get_dcm();
                const base_term_storage& bts = m_surface_fit_x->get_base_term_storage();

                m_b_fit_ok = m_b_fit_ok && m_surface_fit_y->fit(dcm, bts, u, v, y, sigma_y, subsample);
                m_b_fit_ok = m_b_fit_ok && m_surface_fit_z->fit(dcm, bts, u, v, z, sigma_z, subsample);

                return m_b_fit_ok;
            }

        private:
            p_surfacefit_gj m_surface_fit_x{ nullptr };
            p_surfacefit_gj m_surface_fit_y{ nullptr };
            p_surfacefit_gj m_surface_fit_z{ nullptr };

        };
        // ==============================================================================================
        struct surfacefit_xyz_common_param_fit_handler_lu : public surfacefit_xyz_common_param_fit_handler_base
        {
            surfacefit_xyz_common_param_fit_handler_lu()
            {}

            surfacefit_xyz_common_param_fit_handler_lu(
                p_surfacefit& surface_fit_x,
                p_surfacefit& surface_fit_y,
                p_surfacefit& surface_fit_z)
                : surfacefit_xyz_common_param_fit_handler_base(),
                m_surface_fit_x(std::dynamic_pointer_cast<surface_fit_lu>(surface_fit_x)), // evaluates to nullptr if not suitable
                m_surface_fit_y(std::dynamic_pointer_cast<surface_fit_lu>(surface_fit_y)),
                m_surface_fit_z(std::dynamic_pointer_cast<surface_fit_lu>(surface_fit_z))
            {}

            virtual bool fit(const vec& u, const vec& v, const vec& x, const vec& y, const vec& z, const vec& sigma, const vec_b& subsample) override
            {
                m_b_fit_ok = m_surface_fit_x->fit(u, v, x, sigma, subsample);

                const matrix<double>& LUcovar = m_surface_fit_x->LUcovar();
                const vec_i& ppidx = m_surface_fit_x->ppidx();
                const base_term_storage& bts = m_surface_fit_x->get_base_term_storage();

                m_b_fit_ok = m_b_fit_ok && m_surface_fit_y->fit(LUcovar, ppidx, bts, u, v, y, sigma, subsample);
                m_b_fit_ok = m_b_fit_ok && m_surface_fit_z->fit(LUcovar, ppidx, bts, u, v, z, sigma, subsample);

                return m_b_fit_ok;
            }

            virtual bool fit(const vec& u, const vec& v, const vec& x, const vec& y, const vec& z, const double& sigma, const vec_b& subsample) override
            {
                m_b_fit_ok = m_surface_fit_x->fit(u, v, x, sigma, subsample);

                const matrix<double>& LUcovar = m_surface_fit_x->LUcovar();
                const vec_i& ppidx = m_surface_fit_x->ppidx();
                const base_term_storage& bts = m_surface_fit_x->get_base_term_storage();

                m_b_fit_ok = m_b_fit_ok && m_surface_fit_y->fit(LUcovar, ppidx, bts, u, v, y, sigma, subsample);
                m_b_fit_ok = m_b_fit_ok && m_surface_fit_z->fit(LUcovar, ppidx, bts, u, v, z, sigma, subsample);

                return m_b_fit_ok;
            }

            virtual bool fit(const vec& u, const vec& v, const vec& x, const vec& y, const vec& z, const double& sigma_x, const double& sigma_y, const double& sigma_z, const vec_b& subsample) override
            {
                m_b_fit_ok = m_surface_fit_x->fit(u, v, x, sigma_x, subsample);

                const matrix<double>& LUcovar = m_surface_fit_x->LUcovar();
                const vec_i& ppidx = m_surface_fit_x->ppidx();
                const base_term_storage& bts = m_surface_fit_x->get_base_term_storage();

                m_b_fit_ok = m_b_fit_ok && m_surface_fit_y->fit(LUcovar, ppidx, bts, u, v, y, sigma_y, subsample);
                m_b_fit_ok = m_b_fit_ok && m_surface_fit_z->fit(LUcovar, ppidx, bts, u, v, z, sigma_z, subsample);

                return m_b_fit_ok;
            }

        private:
            p_surfacefit_lu m_surface_fit_x{ nullptr };
            p_surfacefit_lu m_surface_fit_y{ nullptr };
            p_surfacefit_lu m_surface_fit_z{ nullptr };
        };
        // ==============================================================================================
        struct surfacefit_xyz_common_param_fit_handler_chol : public surfacefit_xyz_common_param_fit_handler_base
        {
            surfacefit_xyz_common_param_fit_handler_chol()
            {}

            surfacefit_xyz_common_param_fit_handler_chol(
                p_surfacefit& surface_fit_x,
                p_surfacefit& surface_fit_y,
                p_surfacefit& surface_fit_z)
                : surfacefit_xyz_common_param_fit_handler_base(),
                m_surface_fit_x(std::dynamic_pointer_cast<surface_fit_chol>(surface_fit_x)),// evaluates to nullptr if not suitable
                m_surface_fit_y(std::dynamic_pointer_cast<surface_fit_chol>(surface_fit_y)),
                m_surface_fit_z(std::dynamic_pointer_cast<surface_fit_chol>(surface_fit_z))
            {}

            virtual bool fit(const vec& u, const vec& v, const vec& x, const vec& y, const vec& z, const vec& sigma, const vec_b& subsample) override
            {
                m_b_fit_ok = m_surface_fit_x->fit(u, v, x, sigma, subsample);

                const matrix<double>& CholDecCovar = m_surface_fit_x->CholDecCovar();
                const vec& CholDecCovarDiag = m_surface_fit_x->CholDecCovarDiag();
                const base_term_storage& bts = m_surface_fit_x->get_base_term_storage();

                m_b_fit_ok = m_b_fit_ok && m_surface_fit_y->fit(CholDecCovar, CholDecCovarDiag, bts, u, v, y, sigma, subsample);
                m_b_fit_ok = m_b_fit_ok && m_surface_fit_z->fit(CholDecCovar, CholDecCovarDiag, bts, u, v, z, sigma, subsample);

                return m_b_fit_ok;
            }

            virtual bool fit(const vec& u, const vec& v, const vec& x, const vec& y, const vec& z, const double& sigma, const vec_b& subsample) override
            {
                m_b_fit_ok = m_surface_fit_x->fit(u, v, x, sigma, subsample);

                const matrix<double>& CholDecCovar = m_surface_fit_x->CholDecCovar();
                const vec& CholDecCovarDiag = m_surface_fit_x->CholDecCovarDiag();
                const base_term_storage& bts = m_surface_fit_x->get_base_term_storage();

                m_b_fit_ok = m_b_fit_ok && m_surface_fit_y->fit(CholDecCovar, CholDecCovarDiag, bts, u, v, y, sigma, subsample);
                m_b_fit_ok = m_b_fit_ok && m_surface_fit_z->fit(CholDecCovar, CholDecCovarDiag, bts, u, v, z, sigma, subsample);

                return m_b_fit_ok;
            }

            virtual bool fit(const vec& u, const vec& v, const vec& x, const vec& y, const vec& z, const double& sigma_x, const double& sigma_y, const double& sigma_z, const vec_b& subsample) override
            {
                m_b_fit_ok = m_surface_fit_x->fit(u, v, x, sigma_x, subsample);

                const matrix<double>& CholDecCovar = m_surface_fit_x->CholDecCovar();
                const vec& CholDecCovarDiag = m_surface_fit_x->CholDecCovarDiag();
                const base_term_storage& bts = m_surface_fit_x->get_base_term_storage();

                m_b_fit_ok = m_b_fit_ok && m_surface_fit_y->fit(CholDecCovar, CholDecCovarDiag, bts, u, v, y, sigma_y, subsample);
                m_b_fit_ok = m_b_fit_ok && m_surface_fit_z->fit(CholDecCovar, CholDecCovarDiag, bts, u, v, z, sigma_z, subsample);

                return m_b_fit_ok;
            }

        private:
            p_surfacefit_chol m_surface_fit_x{ nullptr };
            p_surfacefit_chol m_surface_fit_y{ nullptr };
            p_surfacefit_chol m_surface_fit_z{ nullptr };
        };
        // ==============================================================================================
        using p_surface_fit_handler = std::shared_ptr<surfacefit_xyz_common_param_fit_handler_base>;
        using surface_fit_handler_base_map = std::map<surface_fit::type_t, p_surface_fit_handler>;
        // ==============================================================================================
    } // end namespace math
} // end namespace zfs