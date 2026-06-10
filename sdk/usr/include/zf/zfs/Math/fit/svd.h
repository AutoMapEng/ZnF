#pragma once

#include <vector>

#include <zfs/zfsapi.h>

#include <zfs/Math/math/basic.h>

namespace zfs { 
    namespace math {

        /** \class SVD
         * \brief  Class providing functionality for SVD (singular value decomposition, see e.g. numerical recipes).
         *
         * \author Juergen Holzner
         *
         * Class providing functionality for SVD (singular value decomposition, see e.g. numerical recipes).
         * Uses ../fit/eigen.h in member function calcSVD(). Provides functionality for SVDPolyFit.
         *
         */

        template<typename T> class SVD
        {
        public:

            SVD();
            SVD(const std::vector<T>& a, int m_, int n_);
            SVD(const T* const a, int m_, int n_);

            void init(const std::vector<T>& a, int m_, int n_);
            void init(const T* a, int m_, int n_);

            bool solution_found() const;

            int get_n() const;
            int get_m() const;

            const T* const getU() const;
            const T* const getV() const;

            std::vector<T> getw() const;

        protected:

            bool m_bSolution;

            int M, N;

            std::vector<T> u;
            std::vector<T> v;
            std::vector<T> w;

            int calcSVD(int m, int n, T* a, T* _w, T* _v, T* rv1);

            bool decompose();
            bool singular() const;

            T weights_min() const;
            T weights_max() const;

        };
    } // end namespace math
} // end namespace zfs
