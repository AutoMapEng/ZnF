#pragma once
#include "ZfsPrjBasic.h"
#include <zfcore/compiler.h>

#include <zfcore/filename.h>
#include "../zfs/zfvector.h"

#include <list>
#include <math.h>
#include <string>
#include <mutex>

#include <Common/def.h>
SWITCH_WARNINGS_OFF
#include <QObject>
SWITCH_WARNINGS_ON

//##################################### GuardetRegisterVecName
//####################################
class VecName;
class ZFTRMatrix;

class ZFSPRJ_API GuardetRegisterVecName {
    friend class VecName;

private:
    VecName* mGuardetVecName; // Zeiger auf das Objekt

public:
    GuardetRegisterVecName( VecName* vn );

    GuardetRegisterVecName( const GuardetRegisterVecName& o );

    virtual ~GuardetRegisterVecName();

    void addPointer( VecName* vp );

    VecName* getPointer() const { return mGuardetVecName; }

protected:
    void pointerDeleted() { mGuardetVecName = NULL; }
};

//##################################### VecName
//###################################################
namespace tinyxml2 {
    class XMLNode;
    class XMLElement;
} // namespace tinyxml2

/**
@brief double vector with name
*/
class ZFSPRJ_API VecName : public QObject, public ZFVec3d
{
    friend class GuardetRegisterVecName;

protected:
    std::list<GuardetRegisterVecName*> mGuardList;
    std::mutex mutGuardedList;

    double mHightOffset = 0; //!< Offset wert fur das Target zur Erfassten Possition.
                        //!< Der Wert ist bereits in der z Komponente verrechnet

    bool use = true; //!< Use this target

public:
    enum flagsTarget // Flags
    {
        F_CHILD = 1, // targetvalues are from a child scan with a higher resolution
        F_ZFC = 2,   // targets are imported from a zfc file
        F_UNCAL = 4, //
        F_SECONDLAYER =
        8,           // target is in second layer if doublelayer scan was taken
        F_NOTUSE = 0x10, // do not use for registration
        F_ANNOTATION = 0x20, // Target taken from the slam annotations 

        F_AUTOFIT = 0x100,  // Targets are fittet based on a list of already defined
                            // targetpositions.
        F_AUTOFIND = 0x400, //  target was automaticaly found, the algorithm will
                            //  find too much targets.

        F_PRISM1 = 0x200, // Prisma Target auf dem Scanner Position 1
        F_PRISM2 = 0x800, // Prisma Target auf dem Scanner Position 1
    };

    zf::FileName name; //!< Name of the point

    // additional
    double quality = 200; //!< quality of the point 1.0 is best
    bool opt = false;       //!< target disabled by optimize default = false
    long flags = 0;     //!< additional flags (flagsTarget)
    int nrFound = 1;    //!< Anzahl der zusammengerechneten Targets (findet Verwendung
                    //!< bei der Berechnung des durchschnitts)
    int vpScanNr = 0;   //!< Nummer des Scans im VP (wenn ein VP mehr als einen scan
                    //!< hat)
    bool ok = false;        //!< flag point ok
    int typ = -1;        //!< type of the target if point is a target
    double u = 0;       //!< pixel in scanfile
    double v = 0;       //!< line in scanfile
    double timestamp = 0;  //!< timestamp fuer slam daten
    double std_dev = 0.01; //Genauigkeit des Targets

protected:
    void addGuardedPointer( GuardetRegisterVecName* pointer );
    void deleteGuardedPointer( GuardetRegisterVecName* pointer );

    void init(const zf::FileName& n, double a = 0., double b = 0., double c = 0.,
        long f = 0, int i = 0, int = 0, int t = 0, int qual = 200)
    {
        set(a, b, c);
        name = n;
        flags = f;
        nrFound = i;
        use = true;
        opt = false;
        ok = false;
        quality = qual;
        typ = t;
    }

public:
    VecName() : QObject()
    {
        init( L"");
    }
    VecName( const std::string& n, double a = 0., double b = 0., double c = 0.,
             long f = 0, int i = 0, int = 0, int t = 0, int qual = 200 ) : QObject()
    {
        init(n, a, b, c, f, i, 0, t, qual);
    }
    VecName( const std::wstring& n, double a = 0., double b = 0., double c = 0.,
             long f = 0, int i = 0, int = 0, int t = 0, int qual = 200 ) : QObject()
    {
        init(n, a, b, c, f, i, 0, t, qual);
    }    

    /**
     *   Kopierkonstruktor
     *   Das Objekt uebernimmt alle GuardetPonter. Die Verlinkungen zum
     * uebergebenen Objekt werden geloescht.
     */
    VecName( const VecName& _v ) : QObject()
    {
        u = _v.u;
        v = _v.v;
        setXYZ( _v.xyz() );
        name = _v.name;
        flags = _v.flags;
        nrFound = _v.nrFound;
        use = _v.use;
        opt = _v.opt;
        ok = _v.ok;
        quality = _v.quality;
        typ = _v.typ;
        vpScanNr = _v.vpScanNr;

        mHightOffset = _v.mHightOffset;
        timestamp = _v.timestamp;
        std_dev = _v.std_dev;
    }
    virtual ~VecName();

    VecName& operator=( const VecName& );

    void setXYZToWorld( const ZFTRMatrix& matrix );
    void setX( double x_ ) { x() = x_; }
    void setY( double y_ ) { y() = y_; }
    void setZ( double z_ ) { z() = z_; }

    /// sets xyz and orginal xyz at once
    void set( double x_, double y_, double z_ )
    {
        setXYZ( x_, y_, z_ );
    }

    void setUse( bool b ) { use = b; }
    bool getUse() const { return use; }

    virtual double X() const { return x(); }
    virtual double Y() const { return y(); }
    virtual double Z() const { return z() + mHightOffset; }

    double ZWithoutOffset() const { return z(); }

    const char* NameC() { return name.c_str(); }
    const wchar_t* NameW() const { return name.w_str(); }

    bool checkCasesensitive( const std::wstring& s ) const;

    bool operator<( const VecName& o )
    {
        if( quality == 200 && o.quality <= 100 )
            return true;
        if( o.quality == 200 && quality <= 100 )
            return false;
        if( quality < o.quality )
            return true;
        return false;
    }

    bool operator==( const wchar_t* s ) const
    {
        if( s )
        {
            std::wstring h;
            h = s;
            if( name == h )
                return true;
        }
        return false;
    }

    bool operator==( const std::wstring& s ) const
    {
        if( name == s )
            return true;
        return false;
    }

    bool operator==( const char* s ) const
    {
        if( s )
        {
            std::string h;
            h = s;
            if( name == h )
                return true;
        }
        return false;
    }

    bool operator==( const std::string& s ) const
    {
        if( name == s )
            return true;
        return false;
    }

    bool operator==( const VecName& o ) const
    {
        if( ( name == o.name ) &&
            ( int( x() * 10 + .5 ) == int( o.x() * 10 + .5 ) ) &&
            ( int( y() * 10 + .5 ) == int( o.y() * 10 + .5 ) ) &&
            ( int( Z() * 10 + .5 ) == int( o.Z() * 10 + .5 ) ) )
            return true;
        return false;
    }

    VecName& operator+( const VecName& o )
    {
        if( nrFound == 0 )
            nrFound = 1;
        xyz() += o.xyz();
        nrFound++;
        return *this;
    }
    VecName& operator+=( const VecName& o ) { return *this + o; }

    void sub()
    {
        if( nrFound == 0 )
            nrFound = 1;

        xyz() /= nrFound;
    }

    /**
     *   differenz zweier Targets
     *   \return distanz zwischen den beiden Targets
     *   \param v => objekt mit dem verglichen werden soll
     *   \param dx => rueckgabewert fuer den Unterschied in x
     *   \param dy => rueckgabewert fuer den Unterschied in y
     *   \param dz => rueckgabewert fuer den Unterschied in z
     */
    VecName diff( const VecName& vn ) const
    {
        VecName d;
        d.name = vn.name;
        d.nrFound = vn.nrFound;
        d.x() = X() - vn.X();
        d.y() = Y() - vn.Y();
        d.z() = Z() - vn.Z();
        d.u = d.length();// sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
        d.flags = flags;
        d.typ = typ;
        return d;
    }

    double length() const { return ZFVec3d::length(); }

    virtual void readXml( const tinyxml2::XMLNode* targetNode );
    virtual tinyxml2::XMLElement* writeXml( tinyxml2::XMLNode* targetsNode,
                           bool allParameter ) const;
    virtual void writeXmlToNode( tinyxml2::XMLElement* targetNode,
                                 bool allParameter, bool saveUse) const;

    /**
     *   Rueckgabe des aktuellen hoehenoffsets fuer dieses Target
     */
    double getCorrection() const { return mHightOffset; }

    /**
     *   Setze  des aktuellen hoehenoffsets fuer dieses Target
     */
    void setCorrection( double offset ) { mHightOffset = offset; }
};

typedef std::list<VecName> vecnamelst_t;
