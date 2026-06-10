#pragma once

#include <zfs/zfsapi.h>

#include <vector>

namespace zfs {
    namespace math {

        template<class T> class matrix;
        class surface_poly;

        // =================================================================

        // 3D-surface - Fit based on Gauss-Jordan Algorithm
        // x : input x (position) of length l
        // y : input y (position) of length l
        // z : input z (measurement) of length l
        // fit_poly : polynomial which at output contains the fit coefficients, the polynomial must have been initialized with an order, the order must be known to derive the number of coefficients to be fit
        // covar : covariance matrix for the coefficients, see: numerical recipes in C 2nd ed. , chapter 15.4
        // chisq : total squared error for the fit (squared sum of deviations)
        // ndat: optional: when -1: all data are taken, when any number between 1 and x.size()==y.size()==z.size(): only the specified number of data are taken.

        bool ZFS_API xy2zlfitgj(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z, surface_poly& fit_poly, matrix<double>& covar, double& chisq, int ndat = -1);

        // =================================================================

        // 3D-surface - Fit based on Gauss-Jordan Algorithm, with uncertainties (Standard Deviations) for the "measurments" z
        // x : input x (position) of length l
        // y : input y (position) of length l
        // z : input z (measurement) of length l
        // sig : input expected uncertainties for the measurements z, length of vector l
        // fit_poly : polynomial which at output contains the fit coefficients, the polynomial must have been initialized with an order, the order must be known to derive the number of coefficients to be fit
        // covar : covariance matrix for the coefficients, see: numerical recipes in C 2nd ed. , chapter 15.4
        // chisq : total squared error for the fit (squared sum of deviations)
        // ndat: optional: when -1: all data are taken, when any number between 1 and x.size()==y.size()==z.size(): only the specified number of data are taken.

        bool ZFS_API xy2zlfitgj(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z, const std::vector<double>& sig, surface_poly& fit_poly, matrix<double>& covar, double& chisq, int ndat = -1);

        // =================================================================

        // 3D-surface - Fit based on LU decomposition
        // -- helping function
        // x : input x (position) of length l
        // y : input y (position) of length l
        // fit_poly : polynomial which at output contains the fit coefficients, the polynomial must have been initialized with an order, the order must be known to derive the number of coefficients to be fit
        // covar : input/output : LU decomposed covariance matrix, correct size must be preset
        // ppidx : input/output : partial (row) pivoting index from LU decomposition, correct size must be preset
        // ndat: input : optional: when -1: all data are taken, when any number between 1 and x.size()==y.size()==z.size(): only the specified number of data are taken.

        bool ZFS_API covarLU(const std::vector<double>& x, const std::vector<double>& y, surface_poly fit_poly, matrix<double>& covar_lu, std::vector<int>& ppidx, int ndat = -1);

        // =================================================================

        // 3D-surface - Fit based on LU decomposition
        // -- helping function
        // x : input x (position) of length l
        // y : input y (position) of length l
        // outlier: input :  indicates the outlier 
        // fit_poly : polynomial which at output contains the fit coefficients, the polynomial must have been initialized with an order, the order must be known to derive the number of coefficients to be fit
        // covar : input/output : LU decomposed covariance matrix, correct size must be preset
        // ppidx : input/output : partial (row) pivoting index from LU decomposition, correct size must be preset
        // ndat: input : optional: when -1: all data are taken, when any number between 1 and x.size()==y.size()==z.size(): only the specified number of data are taken.

        bool ZFS_API covarLU(const std::vector<double>& x, const std::vector<double>& y, const std::vector<bool>& outlier, surface_poly fit_poly, matrix<double>& covar_lu, std::vector<int>& ppidx, int ndat = -1);

        // =================================================================

        // 3D-surface - Fit based on LU decomposition 
        // x   : input : (position) of length L
        // y   : input : (position) of length L
        // z   : input : (measurement) of length L
        // sig : input : (measurement noise figure) of length L
        // fit_poly : input/output : polynomial which at output contains the fit coefficients, the polynomial must have been initialized with an order, the order must be known to derive the number of coefficients to be fit
        // chisq : output : total squared error for the fit (squared sum of deviations)
        // ndat: input : optional: when -1: all data are taken, when any number between 1 and x.size()==y.size()==z.size(): only the specified number of data are taken.

        bool ZFS_API xy2zlfitlu(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z, surface_poly& fit_poly, double& chisq, int ndat = -1);
        bool ZFS_API xy2zlfitlu(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z, const std::vector<double>& sig, surface_poly& fit_poly, double& chisq, int ndat = -1);
        // =================================================================

        // 3D-surface - Fit based on LU decomposition with reuse of LU decomposition result from routine covarLU(.)
        // x : input : (position) of length L
        // y : input : (position) of length L
        // z : input : (measurement) of length L
        // fit_poly : input/output : polynomial which at output contains the fit coefficients, the polynomial must have been initialized with an order, the order must be known to derive the number of coefficients to be fit
        // covar_lu : input : LU decomposed covariance matrix
        // ppindx_lu : input : partial (row) pivoting index from LU decomposition
        // chisq : output : total squared error for the fit (squared sum of deviations)
        // ndat: input : optional: when -1: all data are taken, when any number between 1 and x.size()==y.size()==z.size(): only the specified number of data are taken.

        bool ZFS_API xy2zlfitlu(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z, const matrix<double>& covar_lu, const std::vector<int>& ppindx_lu, surface_poly& fit_poly, double& chisq, int ndat = -1);
        // =================================================================

        // 3D-surface - Fit based on LU decomposition with reuse of LU decomposition result from routine covarLU(.)
        // x : input : (position) of length L
        // y : input : (position) of length L
        // z : input : (measurement) of length L
        // outlier: input : data that should be excluded from the fit ( same as the outlier vector used for precalculating the covariance matrix)
        // fit_poly : input/output : polynomial which at output contains the fit coefficients, the polynomial must have been initialized with an order, the order must be known to derive the number of coefficients to be fit
        // covar_lu : input : LU decomposed covariance matrix
        // ppindx_lu : input : partial (row) pivoting index from LU decomposition
        // chisq : output : total squared error for the fit (squared sum of deviations)
        // ndat: input : optional: when -1: all data are taken, when any number between 1 and x.size()==y.size()==z.size(): only the specified number of data are taken.

        bool ZFS_API xy2zlfitlu(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z, const std::vector<bool>& outlier, const matrix<double>& covar_lu, const std::vector<int>& ppindx_lu, surface_poly& fit_poly, double& chisq, int ndat = -1);

        // =================================================================
    } // end namespace math
} // end namespace zfs