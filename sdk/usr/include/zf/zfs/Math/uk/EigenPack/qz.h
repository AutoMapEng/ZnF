// calculate the generalised eigenvalues and eigenvectors of two matrix a and b
//  
//  the generalised eigenproblem is the solution of the equation A*x = l*B*x, where l = lambda (eigenvalues)
//
// if B is nonsingular, the problem could be solved by reducing it to a standard eigenvalue problem: inv(B)*A*x = l*B*x
//  otherwise an alternative algorithm, called the QZ method, is necessary.
//
// qzhes, qzit and qzval must be called in sequence to compute the eigenvalues
// qzvec must be called after to compute the eigenvectors
//
// note: ierr returns a value != 0 if the number of iterations inside the function 
//          is not enough to compute the eigenvalues
//      eps1 is the reference value for the approximations
//
// see qz.cc for more information

void qzhes(const int n, double **a, double **b, double **z);
            
void qzit(const int n, double **a, double **b, double **z, double ierr, double eps1 = 0.0);

void qzval(const int n, double **a, double **b, double *alfr, double *alfi, double *beta, double **z);

void qzvec(const int n, double **a, double **b, double *alfr, double *alfi, double *beta, double **z);
