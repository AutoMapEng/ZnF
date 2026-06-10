#pragma once

#include <zfs/zfsapi.h>

#include <vector>

//-----------------------------------------------------------------------------

class ZFVecf;

class ZFS_API ZFVecd
{
public:
    double x,y,z;

    ZFVecd() { x=y=z=0.; }
    inline ZFVecd( const ZFVecd& vec) : x(vec.x), y(vec.y), z(vec.z){}
    ZFVecd( double xx, double yy, double zz ) { x=xx; y=yy; z=zz; }
    ZFVecd( const ZFVecf &v );
    ZFVecd(const double* p) { if (p) { x = p[0]; y = p[1]; z = p[2]; } else { x = y = z = 0.; } }

    double operator[](int i) { return i==0 ? x : (i==1 ? y : z); }
};

inline const ZFVecd operator+( const ZFVecd& lhs, const ZFVecd& rhs)
{
    ZFVecd ret( lhs);
    ret.x += rhs.x;
    ret.y += rhs.y;
    ret.z += rhs.z;
    return ret;
}

inline const ZFVecd operator/(ZFVecd const& lhs, ZFVecd const& rhs)
{
    ZFVecd ret( lhs);
    ret.x /= rhs.x;
    ret.y /= rhs.y;
    ret.z /= rhs.z;
    return ret;
}

inline const ZFVecd operator/(ZFVecd const& lhs, double div)
{
    ZFVecd ret( lhs);
    ret.x /= div;
    ret.y /= div;
    ret.z /= div;
    return ret;
}


class ZFS_API ZFVecf
{
public:
    float x,y,z;

    ZFVecf() { x=y=z=0.; }
    ZFVecf( float xx, float yy, float zz ) { x=xx; y=yy; z=zz; }
    ZFVecf( const ZFVecd &v ) { x=(float)v.x; y=(float)v.y; z=(float)v.z; }

    float operator[](int i) { return i==0 ? x : (i==1 ? y : z); }
};

inline ZFVecd::ZFVecd(const ZFVecf &v ) { x=v.x; y=v.y; z=v.z; }

typedef std::vector <ZFVecd> pointsd_t;
typedef std::vector <ZFVecd>::iterator pointsd_it;
typedef std::vector <ZFVecf> pointsf_t;
typedef std::vector <ZFVecf>::iterator pointsf_it;


//-----------------------------------------------------------------------------

class ZFS_API ZFFit
{
protected:
    bool ok;

public:
    ZFFit(){ ok = false; }
    virtual ~ZFFit() {}

    virtual bool Fit( pointsd_t& pt ) = 0;
    virtual bool Fit( pointsf_t& pt ) = 0;

    bool Ok() { return ok; }
};


//-----------------------------------------------------------------------------

class ZFS_API ZF3DPoint : public ZFFit
{
    ZFVecd pt;

public:
    ZF3DPoint( double x=0., double y=0., double z=0. ) : ZFFit() { pt.x=x; pt.y=y; pt.z=z; }

    virtual bool Fit( pointsd_t& ) { return false; }
    virtual bool Fit( pointsf_t& ) { return false; }

    double x() const { return pt.x; }
    double y() const { return pt.y; }
    double z() const { return pt.z; }

    void setX( double x ) { pt.x=x; }
    void setY( double y ) { pt.y=y; }
    void setZ( double z ) { pt.z=z; }
    void set( double x, double y, double z ) { pt.x=x; pt.y=y; pt.z=z; }

    double distance( const ZFVecd& point ) const;
    double distance( const ZFVecf& point ) const { return distance(ZFVecd(point)); }
    double distance( double x, double y, double z ) const { return distance(ZFVecd(x,y,z)); }
    double distance( const ZF3DPoint& point ) const { return distance(point.pt); }

    bool isZero() { return x()==0. && y()==0. && z()==0.; }
};

//-----------------------------------------------------------------------------

class ZFS_API ZFFitPlane : public ZFFit
{
public:
    double nv[3];   ///< Normalenvector, must have length 1
    double p0[3];   // Schwerpunkt
    double d0;      // Distance to Offspring, this has negative values when the offspring is on the negative side of nv.

    double stdevFit; /// < std. dev. of points from plane

    double lowEnc;
    bool mRot;
    ZFVecd point;

public:
    ZFFitPlane();
    ZFFitPlane( const ZFFitPlane& other);
    //ZFFitPlane& operator=( ZFFitPlane other);
    ZFFitPlane& operator=( const ZFFitPlane& other);

    virtual bool Fit( pointsd_t& pt );
    virtual bool Fit( pointsf_t& pt );

    void CalcStdDev(pointsd_t& pt);
    void CalcStdDev(pointsf_t& pt);

    void Fit2( pointsd_t& pt );

    void PojektPointToPlane( double& sx, double& sy, double& sz );
    void PojektPointToPlane( ZFVecd& v );
    void PojektPointToPlane( ZFVecf& v );
    void PojektPointToPlane( ZF3DPoint& v );
    double Angle( const ZFFitPlane& plane2 ) const; // Angle between two planes (rad)
    ZF3DPoint cuttingPoint( ZF3DPoint& pointOnFirstPlane, ZFFitPlane& secondPlane );
    double pointOnPlayn( double x, double y );
    double distToPlane( double x, double y, double z ) const;

private:
    void swap( ZFFitPlane& first, ZFFitPlane& second);

    template<typename points_vector_t> void CalcStdDev( const points_vector_t& a_points, const double a_nv[], const double& a_d0, double& a_StdDevPerpDist) const;

};

bool ZFS_API operator==( const ZFFitPlane& lhs, const ZFFitPlane& rhs);  ///< checks equality within 100 times of double epsilon.