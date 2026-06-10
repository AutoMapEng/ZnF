#pragma once

#include <zfs/zfsapi.h>

#include <vector>

namespace zfs {
    namespace math {

        /** @class  LevenbergMarquardt
        *   @brief  Class to solve non-linear problems using Levenberg-Marquardt method
        *   @author Juergen Holzner
        *   Class to solve non-linear problems using Levenberg-Marquardt method
        *   based on numerical recipes implementation
        *   * Standard deviations (sig) for the individual data (y) can be provided.
        */
        class ZFS_API LevenbergMarquardt
        {

        public:
            // -- typedefs
            typedef void (*fittingfunc_t)(double, double[], double*, double[], int);

            // -- constructors
            LevenbergMarquardt();
            ~LevenbergMarquardt();

            LevenbergMarquardt(fittingfunc_t pFittingFunction,
                const double& aThresh,
                const int aNoOfParameters,
                std::vector<double>* const pX,
                std::vector<double>* const pY,
                std::vector<double>* const pSig,
                const std::vector<double>& a_InitialParamVector); //!< input data etc. are set and fitting is immediately conducted

            LevenbergMarquardt(fittingfunc_t pFittingFunction,
                const double& aThresh,
                const std::vector<int>& aParamsFixed,
                const int aNoOfParameters,
                std::vector<double>* const pX,
                std::vector<double>* const pY,
                std::vector<double>* const pSig,
                const std::vector<double>& a_InitialParamVector); //!< input data etc. are set and fitting is immediately conducted

// -- fitting call
            void fit();

            // -- setters 
            // - must all be set apart from: * setParamsFixedVector() if not parameter values shall be held fixed
            //                               * setInitialParameterVector() if start point of parameter search is 0
            // - or one of the above constructors is used.
            void setFittingFunction(fittingfunc_t pFittingFunction);

            void setIterationStopThreshold(const double& aThresh);

            void setParamsFixedVector(const std::vector<int>& aParamsFixed);

            void setNoOfParameters(const int aNoOfParameters);

            void setData(std::vector<double>* const pX,
                std::vector<double>* const pY,
                std::vector<double>* const pSig);

            void setInitialParameterVector(const std::vector<double>& a_InitialParamVector);

            // -- getters
            std::vector<double> getCoVar() const;
            double                   getChiSqr() const;
            std::vector<double> getParamVector() const;
            std::vector<double>      getParamVectorDouble() const;

        protected:
            // -- private members
            fittingfunc_t            m_pFittingFunction;   //!< function providing the fitting function (user supplied)
            int                      m_NoOfParameter;      //!< number of parameter to be fit
            int                      m_NoOfData;           //!< number of data samples
            double                   m_Threshold;          //!< threshold when iteration is stopped
            double                   m_ChiSqr;             //!< result chisqr (local variable is double)
            std::vector<int>         m_ParamsFixed;        //!< Vector indicating which params shall be held fixed
            std::vector<double>* m_pX, * m_pY, * m_pSig; //!< pointers to the data vectors
            std::vector<double> m_ParamVector,        //!< vector keeping the result parameters
                m_CovarVector,        //!< covariance vector from fit
                m_AlphaVector;        //!< curvature matrix

// -- initialization of members
            void initialization();

            // -- routines adopted from numerical recipes for Levenberg-Marquardt Optimization
            static void mrqmin(double x[], double y[], double sig[], int ndata, double a[], int ia[], int ma,
                double* covar, double* alpha, double* chisq,
                fittingfunc_t pFittingFunction, double* alamda);

            static void mrqcof(double x[], double y[], double sig[], int ndata, double a[], int ia[], int ma,
                double* alpha, double beta[], double* chisq,
                fittingfunc_t pFittingFunction);

        };

        /** @class  LevenbergMarquardt3D (e.g. x, y ,z)
        *   @brief  Class to solve non-linear multidimensional problems using Levenberg-Marquardt method
        *   @author Juergen Holzner
        *   Class to solve non-linear multidimensional problems using Levenberg-Marquardt method
        *   based on numerical recipes implementation
        *   * Standard deviations (sig) for the individual data (y) can be provided.
        *   * The fitting function takes an index to the data
        */

        extern std::vector<double>* gpX1, * gpX2, * gpX3; //!< pointers to the position vectors

        class LevenbergMarquardt3D : public LevenbergMarquardt
        {
        public:
            // -- constructors
            LevenbergMarquardt3D();
            ~LevenbergMarquardt3D();

            LevenbergMarquardt3D(fittingfunc_t pFittingFunction,
                const double& aThresh,
                const int aNoOfParameters,
                std::vector<double>* const pX1,
                std::vector<double>* const pX2,
                std::vector<double>* const pX3,
                std::vector<double>* const pY,
                std::vector<double>* const pSig,
                const std::vector<double>& a_InitialParamVector); //!< input data etc. are set and fitting is immediately conducted

            LevenbergMarquardt3D(fittingfunc_t pFittingFunction,
                const double& aThresh,
                const std::vector<int>& aParamsFixed,
                const int aNoOfParameters,
                std::vector<double>* const pX1,
                std::vector<double>* const pX2,
                std::vector<double>* const pX3,
                std::vector<double>* const pY,
                std::vector<double>* const pSig,
                const std::vector<double>& a_InitialParamVector); //!< input data etc. are set and fitting is immediately conducted

        private:

            void initialization(std::vector<double>* const pX1,
                std::vector<double>* const pX2,
                std::vector<double>* const pX3);
        };
    } // end namespace math
} // end namespace zfs