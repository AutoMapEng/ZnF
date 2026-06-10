#ifndef eigenpack_h
#define eigenpack_h

class   EigenPack{
        Vector      mean, meanPt;                   // mean of the whole list (lt)of values
        Vector      eigSNorm;
        Vector      normList;
        double      covarMat[9];        // covariance matrix of the list
        double      *eigVal;                // eigenvalues of the cov. marix
        double      *eigVec;                // corresponding eigenVectors
        int         nrot;                   // nb of rotation done
        int             maskSize;           // region of fitting
        int         noOfEigenValues;    //no of eigenValues
    
        //void          eigenToStochN();

#ifdef CAI_QUADRIC
        double      Q[16], U[6], V[4];
        double      R[9]; //, T[3];
        double      Acano[9], Bcano[3], Ccano;
        double      i1, i2, i3, i4, i5, k1, k2;
        Vector      startAxis, endAxis;
#endif
    
    public:
        Vector      *lt;                //pointer to the beginning of the list
        EigenPack(void);
        EigenPack(int mask,Vector *list);
        EigenPack(int mask,double *inputarray);
        ~EigenPack();
        void jacobi(double *matrix);        //calculate the n eigenvalues
                                                    // and corresponding eigenvectors.
        void generateCovMatrix(double *cov);            //calcul of the covariance matrix
        void generateCovMatrixPT(double *cov);          //calcul of the covariance matrix with mask = total number of points.
        void generateCentralMomentsMatrix(double *cmm);
        double *calEigenPars(int noOfEigenValues, const double *matrix,double *space);
        void    getEigenParams(int noOfEigenValues, const double *, Matrix &, Matrix &);
        void eigsrt();
        double getRadius(int noOfEigenValues, double *matrix,double *space);
        double getAverageRadius(int noOfEigenValues, double *matrix,double *space);
        Vector getMeanVector(void);
        Vector *normaliseList(void);
        void     setListLength(int);
        void     setListPointer(Vector *);
        double getEVRatio(void);
        
#ifdef CAI_QUADRIC
        // Calculates the nonCanonical Summation Matrix (nonCanonicalQuadricMat) from
        // the list of points (lt)
        void    generateSumMatrix(void);

        // Canonicalises the (nonCanonicalQuadricMat) matix into (Acano, Bcano, Ccano)
        void    generateCanonical(void);
        void    getCoefficients(void);
        void    getCoefficientsOrig(void);
        void    normalisePoints();
        void    correctNormalQ();
        void    convertUV2Q();
        void    printFitError();
#endif
};

#endif
