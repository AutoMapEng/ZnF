#pragma once

#include "ZfsPrjBasic.h"

#include <Common/inttypes_.h>
#include <zfs/Math/3d/point_nd.h>
#include <string>
#include <memory>

namespace zfprj
{
    class StatisticsParameterNameMapping;

    // ======================================================

#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle

    class ZFSPRJ_API TargetStatistic
    {
    public:
        enum { Type = 0 };
        virtual int type() const = 0;
        
    public:
        virtual ~TargetStatistic() {}
        virtual std::string getString() const = 0;
        virtual std::wstring getStringHtml( bool additional ) const = 0;
		/**
        *   String zum Speichern der Statistic. Beginnt immer mit dem Type des TargetStatistic Objektes + ;!
        */
		virtual bool set(const std::string& string2parse) = 0;

        static std::shared_ptr<zfprj::TargetStatistic> create(const std::string& str);
        static std::shared_ptr<zfprj::TargetStatistic> copy( const std::shared_ptr<zfprj::TargetStatistic> statistic );
    };

    class ZFSPRJ_API TargetStatisic3D : public TargetStatistic
    {
    protected:
        //! Correlation value in [0,1] that specifies how well the intensities corellate with the
    //! reference target.
        double correlation{};
        //! This measure in [0,1] attempts to quantify the 'quality' of the target as represented by the
        //! points and intensities. This step is performed by estimating how well the target can be
        //! binarizied into a 'black' and a 'white' class.
        double quality{};
        //! This measure in [0,1] attempts to quantify what fraction of the target area is actually
        //! covered by points.
        double coverage{};
        //! This is a very experimental measure in [0,1] that attempts to quantify the precision of the
        //! registration result in Meters. It is probably not very accurate at this time.
        double precision{};

    public:
        enum { Type = 1 };
        int type() const override { return Type; }

    public:
        TargetStatisic3D() {};
        TargetStatisic3D(double correlation_, double quality_, double coverage_, double precision_) : TargetStatistic()
        {
            set(correlation_, quality_, coverage_, precision_);
        }

        virtual ~TargetStatisic3D() {}

        std::string getString() const override;
        std::wstring getStringHtml( bool developer ) const override;


        bool set(const std::string& string2parse);
        void set(double correlation_, double quality_, double coverage_, double precision_);


        int getQuality();
        bool check();

    };


    /**
     \class TargetStatistic
     \brief Holds the fitting statistic result
     \author    Haertl
     \date  25.04.2016
     */
    class ZFSPRJ_API TargetStatisicSperical : public TargetStatistic
    {
    public:
        TargetStatisicSperical(){}

        virtual ~TargetStatisicSperical(){}

        /**
        \brief Clears this object to its blank/initial state.
        \author    Haertl
        \date  25.04.2016
        */
        void clear()
        {
            stat.clear();
        }

        // -- setter
        /**
        \brief Reads the statistic from a file or scan
        \author    Härtl
        \date  02.05.2016
        \return    true if string can be parsed
        */
        bool set( const std::string& string2parse ) override;

        void setResult( const int       NPtsUsedForFit/*,
                        const double&         StdCntrX,
                        const double&         StdCntrY,
                        const double&         StdCntrZ*/ ); // < Chess Targets

        void setResult( const int          NPtsUsedForFit,
                        const int          FracUsedTrgtAreaInPercent,
                        const double     & StdCntrX,
                        const double     & StdCntrY,
                        const double     & StdCntrZ,
                        const double     & MinRadPts,
                        const double     & MaxRadPts,
                        const double     & MeanRadPts,
                        const double     & StdRadPts,
                        const std::string& Comment ); // < Spherical Targets

        // -- getter

        /**
        \brief Gets statistic as a string
        \author    Haertl
        \date  25.04.2016
        \return    The string.
        */
        std::string getString() const override;
        std::wstring getStringHtml( bool bOutputTargetType ) const override;

//        zfs::math::PointND<double, 3> getExpectedStandardDeviationOfTargetCenterPointX() const;
//        zfs::math::PointND<double, 3> getExpectedStandardDeviationOfTargetCenterPointY() const;
//        zfs::math::PointND<double, 3> getExpectedStandardDeviationOfTargetCenterPointZ() const;
//        std::string                getFitComment() const;
        int                        getNumberOfPointsUsedForFit() const;

//        std::string getTargetType() const;

        double getStandardDeviationOfPointsFromSphericalSurface() const;
        double getMeanDeviationOfPointsFromSphericalSurface() const;
        double getMinDeviationOfPointsFromSphericalSurface() const;
        double getMaxDeviationOfPointsFromSphericalSurface() const;

        int getFractionUsedTargetArea() const; // < in percent 

    public:
        enum { Type = 2 };
        int type() const override { return Type; }

    protected:
        std::string stat;     ///< statistic as string

    protected:

        // -- helping functions
        bool parse_string( const std::string& aString2Parse, const std::string& aParamNameShort, std::string& aValue , std::string& aUnit) const;

        double                    getDoubleFromString   (const StatisticsParameterNameMapping& aParamName) const;
        zfs::math::PointND<double,3> getPointInfoFromString(const StatisticsParameterNameMapping& aParamName) const;
        int                       getIntFromString      (const StatisticsParameterNameMapping& aParamName) const;
        std::string               getStringFromString   (const StatisticsParameterNameMapping& aParamName) const;

        bool                      processFoundParam( const StatisticsParameterNameMapping& aParameterMapping, size_t& aStringPos, std::wstring& aParameterOutString, const bool bOutputTargetType= false ) const;
        std::wstring              convValue(const std::string& aValue, const std::string& aUnit) const;


    };

#pragma warning(pop)

    // ======================================================

} // end namespace
