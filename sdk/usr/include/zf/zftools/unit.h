#pragma once
#include "zftools_api.h"
#include <zfcore/compiler.h>
#ifdef _WINDOWS
#pragma warning(disable: 4273)
#endif

#include <math.h>
#include <vector>
#include <string>


class ZFTOOLS_API ZFUnit
{
protected:
    const std::string digKey;
    int mPrecision{};

public:
    enum eFormat { DOUBLE, FEETINCH, FRACTION };

public:
    ZFUnit(const std::string &digKey, int precision = 0) : digKey(digKey), mPrecision(precision) {}
    virtual ~ZFUnit() {}

    static const char* UnitName() { return "?"; }

    virtual const char* Name() const = 0;
    virtual const char* Unit() const = 0;
    virtual const wchar_t* UnitW() const = 0;
    virtual const char* UnitSmall() const = 0;
    virtual const wchar_t* UnitSmallW() const = 0;
    virtual double fromMeter( double ) const = 0;
    virtual double smallFromMeter( double ) const = 0;
    virtual double toMeter( double ) const = 0;
    virtual double toMeter( const std::string& ) const = 0;
    virtual double smallToMeter( const std::string& txt ) const =0;
    virtual double smallToMeter( double ) const = 0;
    virtual std::string fromMeterToString( double meter, bool printUnit=true ) const = 0;
    virtual std::string fromMeterToStringSmall( double meter, bool printUnit=true ) const = 0;
    virtual const char* getValidatorRegEx() const = 0;

    virtual ZFUnit::eFormat getFormat() const = 0;

    virtual int precisionSmall() const = 0;
    int precision() const { return mPrecision; }
    int setPrecision( int p ) { int old=mPrecision; mPrecision = p; return old;}

    virtual const char* digitsKey() const { return digKey.c_str(); }

private:
    ZFUnit& operator=( const ZFUnit&){ return (*this);}
};


class ZFTOOLS_API ZFUnitMeter : public ZFUnit
{
public:
    ZFUnitMeter() : ZFUnit("DigMeter", 4) {}
    static const char* UnitName() { return "Meter"; }

    const char* Name() const override { return UnitName(); }
    const char* Unit() const override { return "m"; }
    const wchar_t* UnitW() const override { return L"m"; }
    const char* UnitSmall() const override { return "mm"; }
    const wchar_t* UnitSmallW() const override { return L"mm"; }
    double fromMeter( double v ) const override { return v; }
    double smallFromMeter( double v ) const override { return v * 1000.; }
    double toMeter( double v ) const override { return v; }
    double toMeter( const std::string& txt ) const override { return atof( txt.c_str()); }
    double smallToMeter( const std::string& txt ) const override { return atof( txt.c_str() ) / 1000; }
    double smallToMeter( double v ) const override { return v / 1000.; }
    std::string fromMeterToString( double meter, bool printUnit=true ) const override;
    std::string fromMeterToStringSmall( double meter, bool printUnit=true ) const override;
    ZFUnit::eFormat getFormat() const override { return ZFUnit::DOUBLE; }
    const char* getValidatorRegEx() const override { return ""; }
    int precisionSmall() const override { return 1; }

private:
    ZFUnitMeter& operator=( const ZFUnitMeter&){ return (*this);}
};

/**
    Milimeter
*/
class ZFTOOLS_API ZFUnitMM : public ZFUnit
{
public:
    ZFUnitMM() : ZFUnit("DigMM", 1) {}
    static const char* UnitName() { return "Millimeter"; }

    const char* Name() const override { return UnitName(); }
    const char* Unit() const override { return "mm"; }
    const wchar_t* UnitW() const override { return L"mm"; }
    const char* UnitSmall() const override { return "mm"; }
    const wchar_t* UnitSmallW() const override { return L"mm"; }
    double fromMeter( double v ) const override { return v * 1000.; }
    double smallFromMeter( double v ) const override { return fromMeter(v); }
    double toMeter( double v ) const override { return v / 1000.; }
    double toMeter( const std::string& txt ) const override { return atof( txt.c_str()) / 1000.; }
    double smallToMeter( const std::string& txt ) const override { return toMeter( txt ); }
    double smallToMeter( double v ) const override { return toMeter( v ); }
    std::string fromMeterToString( double meter, bool printUnit=true ) const override;
    std::string fromMeterToStringSmall( double meter, bool printUnit=true ) const override;

    ZFUnit::eFormat getFormat() const override { return ZFUnit::DOUBLE; }
    const char* getValidatorRegEx() const override { return ""; }
    int precisionSmall() const override { return 1; }

private:
    ZFUnitMM& operator=( const ZFUnitMM&){return (*this);}
};

class ZFTOOLS_API ZFUnitInch : public ZFUnit
{
protected:
    ZFUnitInch(const std::string &digKey, int precision) : ZFUnit(digKey, precision) {}
public:
    ZFUnitInch() : ZFUnit("DigInch", 2) {}
    static const char* UnitName() { return "Inch"; }

    const char* Name() const override { return UnitName(); }
    const char* Unit() const override { return "in"; }
    const wchar_t* UnitW() const override { return L"in"; }
    const char* UnitSmall() const override { return "in"; }
    const wchar_t* UnitSmallW() const override { return L"in"; }
    double fromMeter( double meter )const override { return meter*(1/0.0254); }
    double smallFromMeter( double meter ) const override { return fromMeter( meter ); }
    double toMeter( double in) const override { return in*0.0254; }
    double toMeter( const std::string& txt ) const override;
    double smallToMeter( double in ) const override { return toMeter( in ); }
    double smallToMeter( const std::string& txt ) const override { return toMeter( txt ); }
    std::string fromMeterToString( double meter, bool printUnit=true ) const override;
    std::string fromMeterToStringSmall( double meter, bool printUnit=true ) const override;
    ZFUnit::eFormat getFormat() const override { return ZFUnit::DOUBLE; }
    const char* getValidatorRegEx() const override { return ""; }

    int precisionSmall() const override { return 3; }

private:
    ZFUnitInch& operator=( const ZFUnitInch&){return (*this);}
};

class ZFTOOLS_API ZFUnitInchFraction : public ZFUnitInch
{
public:
    int precisitonValue{33};

public:
    ZFUnitInchFraction() : ZFUnitInch("DigInchFraction", 2), precisitonValue(33) {}
    static const char* UnitName() { return "Inch fraction"; }

    const char* Name() const override { return UnitName(); }
    std::string fromMeterToString( double meter, bool printUnit=true ) const override;
    std::string fromMeterToStringSmall( double meter, bool printUnit=true ) const override { return fromMeterToString( meter, printUnit ); }
    ZFUnit::eFormat getFormat() const override { return ZFUnit::FRACTION; }
    const char* getValidatorRegEx() const override { return " "; }

    int precisionSmall() const override { return 3; }

private:
    ZFUnitInchFraction& operator=( const ZFUnitInchFraction&){return (*this);}
};


class ZFTOOLS_API ZFUnitFoot : public ZFUnit
{
protected:
    ZFUnitFoot(const std::string &digKey, int precision) : ZFUnit(digKey, precision) {}

public:
    ZFUnitFoot() : ZFUnit("DigFoot", 4) {}
    static const char* UnitName() { return "Feet"; }

    const char* Name() const override { return UnitName(); }
    const char* Unit() const override { return "ft"; }
    const wchar_t* UnitW() const override { return L"ft"; }
    const char* UnitSmall() const override { return "in"; }
    const wchar_t* UnitSmallW() const override { return L"in"; }
    double fromMeter( double meter) const override { return meter*(1./0.3048); }
    double smallFromMeter( double meter ) const override { return fromMeter( meter )*12; }
    double toMeter( double foot) const override { return foot*0.3048; }
    double toMeter( const std::string& txt ) const override;
    double smallToMeter( const std::string& txt ) const override;
    double smallToMeter( double foot ) const override { foot = foot / 12; return toMeter( foot ); }
    std::string fromMeterToString( double meter, bool printUnit=true ) const override;
    std::string fromMeterToStringSmall( double meter, bool printUnit=true ) const override;
    ZFUnit::eFormat getFormat() const override { return ZFUnit::DOUBLE; }
    const char* getValidatorRegEx() const override { return ""; }

    int precisionSmall() const override { return 3; }

private:
    ZFUnitFoot& operator=(const ZFUnitFoot&){return (*this);}
};

class ZFTOOLS_API ZFUnitFootInch : public ZFUnitFoot
{
protected:
    bool mFraction{};
    ZFUnitFootInch(const std::string &digKey, int precision) : ZFUnitFoot(digKey, precision), mFraction(false) {}

public:
    ZFUnitFootInch() : ZFUnitFoot("DigFootInch", 4), mFraction(false) {}
    static const char* UnitName() { return "Feet and Inches"; }

    const char* Name() const override { return UnitName(); }
    double toMeter( const std::string& txt ) const override ;
    std::string fromMeterToString( double meter, bool printUnit=true ) const override ;
    ZFUnit::eFormat getFormat() const override { return ZFUnit::FEETINCH; }

private:
    ZFUnitFootInch& operator=(const ZFUnitFootInch&){return (*this);}
};

class ZFTOOLS_API ZFUnitFootFraction : public ZFUnitFootInch
{
public:
    ZFUnitFootFraction() : ZFUnitFootInch("DigFootFraction", 4) { mFraction = true;}
    static const char* UnitName() { return "Feet and Inches fraction"; }

    const char* Name() const override { return UnitName(); }
    const char* getValidatorRegEx() const override { return ""; }
    std::string fromMeterToString( double meter, bool printUnit=true ) const override;
    std::string fromMeterToStringSmall( double meter, bool printUnit=true ) const override  { return fromMeterToString( meter, printUnit ); }
    int precisionSmall() const override { return precision(); }

private:
    ZFUnitFootFraction& operator=( const ZFUnitFootFraction&){ return (*this);}
};

class ZFTOOLS_API ZFUnitUSSurveyFoot : public ZFUnitFoot
{
public:
    ZFUnitUSSurveyFoot() : ZFUnitFoot("DigSurveyFoot", 4) {}
    static const char* UnitName() { return "USSurveyFeet"; }

    const char* Name() const override { return UnitName(); }
    double toMeter( double foot) const override { return foot*1200./3937.; }
    ZFUnit::eFormat getFormat() const override { return ZFUnit::DOUBLE; }
    int precisionSmall() const override  { return 3; }
    const char* getValidatorRegEx() const override  { return ""; }

private:
    ZFUnitUSSurveyFoot& operator=( const ZFUnitUSSurveyFoot&){ return (*this);}
};

class ZFTOOLS_API ZFUnitUSSurveyFootInch : public ZFUnitFootInch
{
public:
    ZFUnitUSSurveyFootInch() : ZFUnitFootInch("DigSurveyFootInch", 4) {}
    static const char* UnitName() { return "USSurveyFeet and Inch"; }

    const char *Name() const override { return UnitName(); }
    double fromMeter(double meter) const override { return meter * 3937 / 1200.; }
    double toMeter(const std::string &txt) const override { return ZFUnitFootInch::toMeter(txt); };
    double toMeter( double foot) const override { return foot*1200./3937; }
    ZFUnit::eFormat getFormat() const override  { return ZFUnit::FEETINCH; }
    const char* getValidatorRegEx() const override  { return ""; }

private:
    ZFUnitUSSurveyFootInch& operator=( const ZFUnitUSSurveyFootInch&){ return (*this);}
};

class ZFTOOLS_API ZFUnitYard : public ZFUnit
{
public:
    ZFUnitYard() : ZFUnit("Yard", 3) {}
    static const char* UnitName() { return "Yard"; }

    const char* Name() const override { return UnitName(); }
    const char* Unit() const override { return "yd"; }
    const wchar_t* UnitW() const override { return L"yd"; }
    const char* UnitSmall() const override { return "yd"; }
    const wchar_t* UnitSmallW() const override { return L"yd"; }
    double fromMeter(double meter) const override { return meter*(1 / .9144); }
    double smallFromMeter(double meter) const override { return fromMeter(meter); }
    double toMeter(double y) const override { return y*.9144; }
    double toMeter(const std::string& txt) const override ;
    double smallToMeter(double y) const override { return toMeter(y); }
    double smallToMeter(const std::string& txt) const override { return toMeter(txt); }
    std::string fromMeterToString(double meter, bool printUnit = true) const override;
    std::string fromMeterToStringSmall(double meter, bool printUnit = true) const override;
    ZFUnit::eFormat getFormat() const override { return ZFUnit::DOUBLE; }
    const char* getValidatorRegEx() const override { return ""; }

    int precisionSmall() const override { return 3; }

private:
    ZFUnitYard& operator=( const ZFUnitYard&){ return (*this);}

};

//=====================================================================================================
#pragma warning( push )
#pragma warning( disable : 4251 )

class ZFTOOLS_API ZFUnits
{
protected:
    std::vector<ZFUnit*> units;
    ZFUnit* current{};
    bool mImport{};

public:
    ZFUnits();
    ZFUnits( const ZFUnits& );
    virtual ~ZFUnits();

    ZFUnits& operator=(const ZFUnits& other);

    int Init( const bool imp, const char* sel=NULL );

    bool Change( const std::string& name );								///< set the current unit
    const char* Current() const { return current->Name(); }               ///< get the current unit as name
    const char* Unit() const { return current->Unit(); }                  ///< e.g. m
    const wchar_t* UnitW() const { return current->UnitW(); }
    const char* UnitSmall() const { return current->UnitSmall(); }        ///< e.g. mm
    const wchar_t* UnitSmallW() const { return current->UnitSmallW(); }

    // current selected unit

    /// convert from internal meter to a string e.g. '12.345ft'
    std::string fromMeterToString(double v, bool printUnit = true) const { return current->fromMeterToString(v, printUnit); }

    /// convert from internal meter to a string e.g. '12345inch'
    std::string fromMeterToStringSmall(double v, bool printUnit = true) const { return current->fromMeterToStringSmall(v, printUnit); }

    double fromMeter(double v) const { return current->fromMeter(v); }
    double toMeter(double v) const  { return current->toMeter(v); }
    double toMeter(const std::string& txt) const { return current->toMeter(txt); }

    double smallFromMeter(double v) const { return current->smallFromMeter(v); }
    double smallToMeter( double v ) const { return current->smallToMeter(v); }
    double smallToMeter( const std::string& txt ) const { return current->smallToMeter(txt); }

    ZFUnit::eFormat getFormat() const { return current->getFormat(); }
    std::string fromSquareMeterToString( double squareMeter) const;

    int Units() const;                                ///< Number of supported units
    ZFUnit* CurrentPtr() { return current; }    ///< Current unit pointer
    const ZFUnit* CurrentPtr() const { return current; }    ///< Current unit pointer
    ZFUnit& operator[](unsigned int idx);       ///< Read one of the supported units
    ZFUnit& getMeterUnit() { return *units[0]; }
    const ZFUnit& getMeterUnit() const { return *units[0]; }

    std::vector<std::string> getSupportedUnits() const;


    int precision() const { return current->precision(); }
    int precisionSmall() const { return current->precisionSmall(); }
    /// set new precision, return old setting
    int setPrecision(int prec){ return current->setPrecision(prec);}


private:
    void deleteUnits();
    void copyPrecisionValues( const ZFUnits& other);
};
#pragma warning( pop )

//=====================================================================================================

// get access to the global object for the units
namespace zf
{
    ZFTOOLS_API ZFUnits& units4General(); // like visualisation
    ZFTOOLS_API ZFUnits& units4Import();
    ZFTOOLS_API ZFUnits& units4Export();
}

ZFTOOLS_API ZFUnits& getInitializedUnits(); ///< returns ::units, but initializes it if it is not initialized.

// use units4... instead
ZFTOOLS_API extern ZFUnits units;		// global unit in LaserControl fuer die Ausgaben z.B Messen
ZFTOOLS_API extern ZFUnits unitsImport;	// global unit fuer Import
ZFTOOLS_API extern ZFUnits unitsExport;	// global unit fuer Export


//=====================================================================================================

namespace zftools
{
    enum temperatur_t { T_CELSIUS, T_FAHRENHEIT, END_OF_TEMPERATURE_ENUM };
    ZFTOOLS_API void TemperaturSetFormat( temperatur_t );
    ZFTOOLS_API temperatur_t TemperaturGetFormat();
    ZFTOOLS_API void setTemperaturePrecision( int digitsAfterDecimalPoint);
    ZFTOOLS_API int getTemperaturePrecision();
    ZFTOOLS_API std::wstring Temperatur2Str( double celsius );
    ZFTOOLS_API std::wstring TemperaturDifference2Str( double kelvin);
    ZFTOOLS_API std::wstring getUnitString(); ///< Returns only the unit as a string (e.g. "�C")
    ZFTOOLS_API std::wstring getFullUnitName( temperatur_t unit);
    ZFTOOLS_API double fromCelsiusToCurrentUnit( double celsius);
    ZFTOOLS_API double fromCurrentUnitToCelsius(double temperature);
    ZFTOOLS_API double temperatureDifferencefromKelvinToCurrentUnit( double kelvin);
    ZFTOOLS_API std::string getIniKeyTemperatureUnit();
    ZFTOOLS_API std::string getIniKeyTemperaturePrecision();

    class TemporaryTemperatureFormatChanger
    {
    private:
        temperatur_t oldTemp;
        int oldDigits;

    public:
        TemporaryTemperatureFormatChanger( temperatur_t unit, int nDigits)
        {
            oldTemp = unit;
            oldDigits = nDigits;
            TemperaturSetFormat(unit);
            setTemperaturePrecision(nDigits);
        }

        ~TemporaryTemperatureFormatChanger()
        {
            TemperaturSetFormat(oldTemp);
            setTemperaturePrecision(oldDigits);
        }
    };


    class TimeUnits
    {
    public:
        static std::wstring getCurrentUnit(){ return L"s";}
        static int getPrecision(){ return 10;}
    };

} // end namespace zftools

//=====================================================================================================

