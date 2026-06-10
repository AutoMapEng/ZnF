#pragma once

#include <vector>

namespace zfs {
    namespace math {

        // ==============================================================================================

        using vec_b = std::vector<bool>;
        using vec_i = std::vector<int>;
        using tuple_i = std::pair<int, int>;

        // ==============================================================================================
        // class for keeping the polynomial base terms for faster calculations
        // ==============================================================================================
        class base_term_storage
        {
        public:

            base_term_storage() {}

            void set(const surface_for_error_analysis& fit_poly, size_t n_base_terms)
            {
                m_n_base_terms = n_base_terms;
                m_base_term_data.resize(m_n_base_terms);
                for (size_t n = 0; n < m_n_base_terms; ++n)
                    m_base_term_data[n].resize(fit_poly.n_coefs(), 0.);
            }

            vec& operator()(size_t n)
            {
                return m_base_term_data[n];
            }

            const vec& operator()(size_t n) const
            {
                return m_base_term_data[n];
            }

            const size_t& n_base_terms() const
            {
                return m_n_base_terms;
            }

        private:

            size_t           m_n_base_terms{ 0 };
            std::vector<vec> m_base_term_data;

        };

        // ==============================================================================================
    } // end namespace math
} // end namespace zfs