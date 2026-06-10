#pragma once

#include <zfs/zfsapi.h>

#include <zfs/Math/math/lin_alg_common.h>



short ZFS_API  leastSqrs(double *x1, double *y1, double *x2, double *res,
                             int n, int baseId, int baseDim);


short ZFS_API transImgAtoImgAT(BYTE *IMGA, BYTE *IMGB,
                                int NumCols, int NumLines, int baseID,
                                double *fx, double *fy,
                                int X, int Y, int width, int height);


short ZFS_API MLMP_addIMG(BYTE *IMGA, BYTE *IMGB, BYTE *IMGC,
                             long cols,long rows,long C);

short ZFS_API MLMP_subIMG(BYTE *IMGA, BYTE *IMGB, BYTE *IMGC,
                             long cols,long rows,long C);

short ZFS_API cropIMG(BYTE *IMGA, BYTE *IMGB, long rows,long cols, long cutX,
                         long cutY, long cutWidth, long cutHeight);

short ZFS_API Copy_IMGA_2_IMGB(BYTE *IMGA, BYTE *IMGB, long NumCols, long NumLines,
                                    long X, long Y, long width, long height);

short ZFS_API plane2paraForm(matrix_t *R, double d, matrix_t *paraForm);

short ZFS_API calcDomain(long w1, long h1, double d,
                            double *R1, double *R3, double *I2NI,
                            double *min_u, double *min_v, double *max_u, double *max_v);

short ZFS_API proj_IMGA_AND_IMGB_2_IMGC(BYTE *IMG1, BYTE *IMG2,
                                           BYTE *IMG3, BYTE *IMG4,
                                            long *w1, long *h1,
                                            long *w2, long *h2,                                         double *R1, double *R2, double *R3,
                                            double *I2NI, long recPolyID,
                                            long *dMap_All, long *dMap_1, long *dMap_2,
                                            double *x1, double *y1, long *nb_corr_pnts,
                                            double *x2, double *y2);


short ZFS_API ZYZ_Move_IR3(double alpha, double beta, double gamma,
                              double tx, double ty, double tz, matrix_t *M);

short ZFS_API segm_IMG_polyMethod(BYTE *IMG1, BYTE *IMG2, long w1, long h1,
                                     long recPolyID, double thr1, double thr2,
                                     double *val, BYTE red, BYTE green, BYTE blue,
                                     long foreGroundFlag);

double ON_Poly(double x, double y, int op, int poly_type);

short ZFS_API recLocally_create_matrix(matrix_t *m_At, int poly_type);

short ZFS_API segm_IMG_polyMethod(BYTE *IMG1, BYTE *IMG2, long w1, long h1,
                                     long recPolyID, double thr1, double thr2,
                                     double *val, BYTE red, BYTE green, BYTE blue);

short ZFS_API recLocally_calc_EV(int poly_type, matrix_t *m_a, matrix_t *ev,
                                    double *lambda1, double *lambda2);

short ZFS_API smooth_IMG(BYTE *IMG1, BYTE *IMG2, long w1, long h1,
                                     long ID);

double getPolyVal(double x, double y, int ID, double *a);

short ZFS_API stNB_Min_along_PC(matrix_t *m_pc, matrix_t *m_p, int ID,
                                   long backGroundFlag, double *res_u, double *res_v);

short ZFS_API distMap(BYTE *imgA, long *dMap, long threshhold,
                         long sx, long sy);

short ZFS_API poly_Transf_IMGA_IMGB_2_IMBC(BYTE *IMGA, BYTE *IMGB,
                                              BYTE *IMGC, BYTE *IMGD,
                                              long NumCols, long NumRows, long baseID,
                                              double *fx, double *fy,
                                              double *ifx, double *ify,
                                              long *dMap, long *area);

short ZFS_API ZYZ_Move_IR3_ret_Dbl(double alpha, double beta, double gamma, double tx,
                                      double ty, double tz, double *dM);

short ZFS_API paintLine(long *dMap, long w1, long h1, double x1, double y1,
                           double x2, double y2);

short ZFS_API  calc_dMap_cut(long *dMap_All, BYTE *IMG1, BYTE *IMG2, BYTE *IMG3, BYTE *IMG4,
                               long w3, long h3, long *nb_border_pnts,
                               double *border_x, double *border_y);

short ZFS_API appoxHesseNormPlane( double *appoxData, int n, double* x, double *y, double *a );

short ZFS_API recLocally_calc_GV(double *X1i, int ID, matrix_t *m_ONB,
                                    int w, int h, int size, BYTE *IMG1,
                                    BYTE *val, int *idx, int col,
                                    matrix_t *b, matrix_t *m_res);

// ZF
int ZFS_API zf_Harris_crit( BYTE *rf_data, int w, int h,
                            double *out_u, double *out_v, double *harrisCrit, int *k,
                            int recPolyID, int flag, int threshold );

int ZFS_API zf_calc_min_along_PCs(matrix_t *m_pc, matrix_t *m_p,
                                  double *pc1_x, double *pc1_y,
                                  double *pc_2x, double *pc2_y  );

int ZFS_API zf_corner_detection( double *harrisCrit, int w, int h,
                            double *out_u, double *out_v, int *k );

int ZFS_API zf_recLocally_calc_GV(double *X1i, int ID, matrix_t *m_ONB,
                                    int w, int h, int size, BYTE *IMG1,
                                    double *val, int *idx, int col,
                                    matrix_t *b, matrix_t *m_res);


int ZFS_API zf_smooth_data(BYTE *IMG1, BYTE *retImg, int w, int h );

short ZFS_API matrix2double(matrix_t *R, double *M);
short ZFS_API double2matrix(matrix_t *R, double *M, int rowdim, int coldim);
