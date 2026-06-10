#ifndef matrix_h
#define matrix_h
#include <stdio.h> //nt
#include <iostream> //nt
using std::cout;
using std::endl;
using std::fstream;
using std::ostream;
using std::istream;
// never in header files using namespace std;
#include "Maths/Vector/vector.h"

#define ArERROR         0
#define ArINFORMATION   1
#define ArQUESTION      2
#define ArWARNING        3
#define ArMESSAGE        4

#ifndef TRUE
#define TRUE    (int)1
#endif

#ifndef FALSE
#define FALSE  (int)0 
#endif


class Matrix{
private:
    int col;
    int row;
    int size;
    double *matrix;

protected:
    // Default message handling
    static int    defaultMessage(char *message, int messageType);     
    static double  defaultDouble;

public:
    static int errNo;

    // Constructors
    Matrix(void);                           // Create an empty matrix
    Matrix(int n);                          // Create an n*n matrix initialised to 0.
    Matrix(int r, int c, double *p);    // Create an object which points to an 
                                                // r*c array, and copy values pointed  p. 
    Matrix(int r, int c, double a=.0);// Create an r*c matrix initialised to a.
    Matrix(const Matrix &A);            // Creates a new matrix which is the 
                                                // same size as A but allocates storage
                                                // at a different location and copies 
                                                // the array.
    Matrix(const Vector &a);            // Convert a vector into a matrix .
                                                // Create a 3*3 rotation matrix from the
                                                // vector a (normalised by the angle).
    ~Matrix(void);                        

    // inquiry methods
    int getRowNumber(void) const;                                             
    int getColNumber(void) const;                                             
    double getValue(int i, int j) const;                                         
    double *getMatrix();                                                              
    int extract(int i, int j, Matrix &, int r=0, int c=0) const;     
    int insert(int i, int j, Matrix &, int r=0, int c=0) const;   
    void    display(int i=0);       // (i=0 ? NORMAL : (i=1 ? MATHEMATICA : null))
    void    display(FILE *);                                                          

    // modification methods 
    void    reset(double val = 0.0);
    void    setMatrix(int, int, double a = 0.0);
    void    setMatrix(int, int, double*);                                        
    void    setMatrixPtr(double *);                                              

    // modification functions 
    friend void   setId(Matrix &);                                            
    friend Matrix   setRandomMatrix(int, int, double, double);

    // operator methods
    double           &operator[](int);
    double           operator[](int) const;
    Matrix           &operator=(Matrix );
    Matrix           &operator=(Vector &);
    Matrix           &operator=(float**);                                        
    Matrix           &operator+=(double);                                        
    Matrix           &operator+=(const Matrix &);                             
    Matrix           &operator-=(double);                                        
    Matrix           &operator-=(const Matrix &);                             
    Matrix           &operator*=(double);                                        
    Matrix           &operator*=(const Matrix &);                             
    Matrix           &operator/=(double);                                        
    Matrix           &operator/=(const Matrix &);                             

    // operator functions
    Matrix & operator()(Matrix &);
    friend int      operator==(const Matrix &, const Matrix &);       
    friend int      operator!=(const Matrix &, const Matrix &);       
    friend Matrix   operator+(const Matrix &, const Matrix &);
    friend Matrix   operator-(const Matrix &, const Matrix &);
    friend Matrix   operator*(const Matrix &, const Matrix &);
    friend Matrix   operator*(const Matrix &, const double);              
    friend Matrix   operator*(const double , const Matrix &);            
    friend Matrix   operator/(const Matrix &, const double);              
    friend ostream& operator<<(ostream& o, const Matrix&);               
    friend istream& operator>>(istream& i, Matrix&);                         

    // miscellaneous methods
    static int      (*messageFunc)(char *message, int messageType);  
    static int      setError(int errorNumber, int messageType);       
    void                svd(Matrix &, Matrix&, Matrix &);
    double          **genNRPtr(void);                                            
    void                symeig(Matrix &, Matrix &);
    void                mul(const Matrix &, Matrix &);
    int             power(Matrix &eigVec, double &eigVal);               

    // related functions
    friend Matrix   ludInverse(const Matrix &);
    friend Matrix   svdInverse(const Matrix &, int *nulsv=NULL);         
    friend Matrix   chdInverse(const Matrix &);
    friend Matrix   inverse(const Matrix &);
    friend Matrix   inv(Matrix &);                                               
    friend void     SVDecmp(Matrix , Matrix *&, Matrix *&, Matrix *&); 
    friend Matrix   transpose(const Matrix &);                               
    friend double   trace(const Matrix &);                                    

    // Homogeneous specific methods
    void                setHMatrix(const Matrix &R, const Matrix &T);
    void                getRT(Matrix &, Matrix&);                                 
    Matrix           invHMatrix(void);                                           

    // Homogeneous specific functions
    friend Matrix   invHMatrix(const Matrix &) ;     // have to make a choice ?

    // Rotation specific methods
    Matrix           matToExp(void);
    Matrix           expToMat(void);

    // Rotation specific functions

    // Vector specific methods
    double           norm(void);                                                     
  
    // Vector specific functions
    friend Vector   matrixToVector(const Matrix &);                       
    friend double   length(const Matrix &);                                  
    friend Matrix   normalise(const Matrix &);                                
    friend double   dotProduct(const Matrix &, const Matrix &, const bool inputsNormalised=false);        
    friend Matrix   crossProduct(const Matrix &u, const Matrix &v2);
};

extern double W_TOLERANCE;      // defined in CCmatrixAnsi/svdinvert.cc 1e-7
void    addOuterProduct(Matrix &, const Matrix &);
int rankdiag(Matrix&);
int invdiag(Matrix &, double tol = W_TOLERANCE);                 
void    inv(Matrix &, const Matrix &, const Matrix &, Matrix&);  

#endif
