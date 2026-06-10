#ifndef vector_h
#define vector_h

#include <iostream> //nt
// never in headder files using namespace std;
#define SMALL_ANGLE 2*3.14/360 //1 degree

class BoundaryPoint;

class FVector;
class Vector;
class PSVector;

// Integer vector class
class IVector
{
public:
    int x, y, z;

    friend std::ostream& operator<<(std::ostream &out, const IVector &v);
    IVector(int i=0, int j=0, int k=0);
    IVector(const FVector &v);
    IVector(const Vector &v);

    IVector& operator=(const FVector &right);
    IVector& operator=(const IVector &right);
    IVector& operator+=(const IVector &right);
    IVector& operator-=(const IVector &right);
    IVector& operator()(const int vx, const int vy, const int vz);
    friend FVector operator*(const IVector &v1, const float s);
    friend Vector operator*(const IVector &v1, const double s);
    friend IVector operator+(const IVector &v1, const IVector &v2);
    friend IVector operator-(const IVector &v1, const IVector &v2);
    friend IVector operator/(const IVector &v1, const int &v2);
};

// Float vector class
class FVector{
public:
    float   x, y, z;

    friend std::ostream& operator<<(std::ostream &out, const FVector &v);
    FVector(float i =0.0, float j=0.0, float k=0.0);
    FVector(const FVector &v);
    FVector(const Vector &v);
    void display (void);
    FVector& operator=(const FVector &right);
    FVector& operator=(const Vector &right);
    FVector& operator+=(const FVector &right);
    FVector& operator-=(const FVector &right);
    FVector& operator()(const float vx, const float vy, const float vz);
    friend FVector operator+(const FVector &v1, const FVector &v2);
    friend FVector operator-(const FVector &v1, const FVector &v2);
    friend FVector operator^(const FVector &v1, const FVector &v2);
    friend FVector operator/(const FVector &v1, const float d);
    friend FVector operator/(const FVector &v1, const int d);
    friend FVector operator*(const float d, const FVector &v1);
    friend FVector operator*(const FVector &v1, const float d);
    friend double  operator*(const FVector &v1, const FVector &v2);
    friend FVector cross(const FVector &v1, const FVector &v2);
    friend double len(const FVector &v1);
    friend FVector normalise(FVector &v1);
    friend double angle(const FVector v1,const  FVector v2);
private:
    };
double dot(const FVector &v1, const FVector &v2, const bool inputsNormalised=false);

// Double Vector class

class Vector{
public:
    double  x, y, z;

    friend std::ostream& operator<<(std::ostream &out, const Vector &v);
    Vector(double i =0, double j=0, double k=0);
    Vector(const Vector &v);
    Vector(const FVector &v);
    void display (void);
    Vector& operator=(const Vector &right);
    Vector& operator=(const FVector &right);
    Vector& operator=(const PSVector &right);
    Vector& operator+=(const Vector &right);
    Vector& operator-=(const Vector &right);
    Vector& operator()(const double vx, const double vy, const double vz);
    void rotateZ(const double alpha);
    friend Vector operator+(const Vector &v1, const Vector &v2);
    friend Vector operator-(const Vector &v1, const Vector &v2);
    friend Vector operator^(const Vector &v1, const Vector &v2);
    friend Vector operator/(const Vector &v1, const double d);
    friend Vector operator/(const Vector &v1, const int d);
    friend Vector operator*(const double d, const Vector &v1);
    friend Vector operator*(const Vector &v1, const double d);
    friend double operator*(const Vector &v1, const Vector &v2);
    friend Vector cross(const Vector &v1, const Vector &v2);
    friend double len(const Vector &v1);
    friend double module2(const Vector &v1);
    friend Vector normalise(Vector &v1);
    friend double angle(const Vector v1,const  Vector v2);
    friend int planeIntersection(const Vector &normal1, const Vector &normal2, const double d1, const double d2, Vector &direction, Vector &position);

private:
};
double dot(const Vector &v1, const Vector &v2, const bool inputsNormalised=false);


// Double SVector inherits Vector
// Nb. this class is now only used in reading v3 XYZim files, since it is the type of two data members of ImageHeaderThree.
class SVector: public Vector{
public:
    double vcm[9];
    SVector(double i =0, double j=0, double k=0);
    //SVector(double *cov, double i=0 , double j=0, double k=0);
    //friend SVector operator*(const double d, const SVector &v1);
    //friend SVector operator+(const SVector &v1, const SVector &v2);
    //friend SVector operator-(const SVector &v1, const SVector &v2);
private:
};


typedef FVector F_Vector;
class FWVector: public FVector{
public:
   float w;
    FWVector(float i =0, float j=0, float k=0, float l =1.0);
    FWVector(const FWVector &v);
    FWVector& operator=(const FWVector &right);
    friend FWVector operator+(const FWVector &v1, const FWVector &v2);
    friend FWVector operator-(const FWVector &v1, const FWVector &v2);
    friend FWVector operator^(const FWVector &v1, const FWVector &v2);
    friend FWVector operator/(const FWVector &v1, const float d);
    friend FWVector operator*(const float d, const FWVector &v1);
    friend FWVector operator*(const FWVector &v1, const float d);
    friend double  operator*(const FWVector &v1, const FWVector &v2);
    friend FWVector cross(const FWVector &v1, const FWVector &v2);
    friend double len(const FWVector &v1);
    friend FWVector normalise(FWVector &v1);
    friend double angle(const FWVector v1,const  FWVector v2);
};
double dot(const FWVector &v1, const FWVector &v2, const bool inputsNormalised=false);

Vector vectorThruPlane(const Vector &point1,const Vector &point2,const Vector &unitV, const double d);
Vector vectorThruTranslatedPlane(const Vector &crossingV,const Vector &eyePt,const Vector &unitV, double d);
int hRotatePlane(double *R, double *T, Vector &uNorm, double &d, Vector &newNorm, double &newd);
double distanceFromPlane(const Vector &point1,const Vector &unitV,const double d);
Vector pointOnPlane(const Vector &point1,const Vector &unitV,const double d);
double orthoDistFrom3DPtToLine(const Vector &, const Vector &, const Vector &);
int getCloudCenter(Vector *vectorArray, int arraySize);
int checkDirection(Vector &normal,Vector &viewPt,BoundaryPoint *array, int vCount);
int checkDirection(Vector &normal,Vector &viewPt,Vector *array, int vCount);

Vector getIntersectionOfTwoLines(const Vector &line1A, const Vector &line1B, const Vector &line2A, const Vector &line2B);

class PVector{
    public:
        double  r;
        double  az;
        double  el;
};

class PSVector{
    public:
        double  r;
        double  az;
        double  el;
        double vcm[9];
};

//
// include to speed up the code in release version
//

#ifdef RELEASE

// IVector class

inline IVector::IVector(int i, int j, int k) : x(i), y(j), z(k) {};

inline IVector::IVector(const Vector &v)
{
    x = (int) v.x;
    y = (int) v.y;
    z = (int) v.z;
}

inline Vector operator*(const IVector &v1, const double s)
{
   return Vector(v1.x * s, v1.y * s, v1.z * s);
}

inline IVector operator+(const IVector &v1, const IVector &v2)
{
   return IVector(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
}

inline IVector operator-(const IVector &v1, const IVector &v2)
{
   return IVector(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z);
}

inline IVector& IVector::operator=(const IVector &right)
{
   x = right.x;
   y = right.y;
   z = right.z;
   return *this;
}

// Vector class

inline Vector::Vector(double i, double j, double k) : x(i), y(j), z(k) {}

inline Vector::Vector(const Vector &v) : x(v.x), y(v.y), z(v.z) {}

inline Vector operator+(const Vector &v1, const Vector &v2)
{
   return Vector(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
}

inline Vector operator-(const Vector &v1, const Vector &v2)
{
   return Vector(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z);
}

inline Vector operator/(const Vector &v1, const double d)
{
   return Vector(v1.x/d, v1.y/d, v1.z/d);
}

inline Vector operator*(const double d, const Vector &v1)
{
   return Vector(v1.x*d, v1.y*d, v1.z*d);
}

inline Vector operator*(const Vector &v1, const double d)
{
   return Vector(v1.x*d, v1.y*d, v1.z*d);
}

inline double operator*(const Vector &v1, const Vector &v2)
{
   return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
}

inline double module2(const Vector &v1)
{
   return (v1.x*v1.x + v1.y*v1.y + v1.z*v1.z);
}

inline Vector& Vector::operator=(const Vector &right)
{
   x = right.x;
   y = right.y;
   z = right.z;
   return *this;
}

#endif // end ifdef RELEASE

#endif
