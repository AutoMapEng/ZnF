#pragma once

#define ELLIPSOID_AIRY					1
#define ELLIPSOID_AUSTRALIAN_NATIONAL	2
#define ELLIPSOID_BESSEL_1841			3
#define ELLIPSOID_BESSEL_1841_NAMBIA	4
#define ELLIPSOID_CLARKE_1866			5
#define ELLIPSOID_CLARKE_1880			6
#define ELLIPSOID_EVEREST				7
#define ELLIPSOID_FISCHER_1960_MERCURY	8
#define ELLIPSOID_FISCHER_1968			9
#define ELLIPSOID_GRS_1967				10
#define ELLIPSOID_GRS_1980				11
#define ELLIPSOID_HELMERT_1906			12
#define ELLIPSOID_HOUGH					13
#define ELLIPSOID_INTERNATIONAL			14
#define ELLIPSOID_KRASSOVSKY			15
#define ELLIPSOID_MODIFIED_AIRY			16
#define ELLIPSOID_MODIFIED_EVEREST		17
#define ELLIPSOID_MODIFIED_FISCHER_1960	18
#define ELLIPSOID_SOUTH_AMERICAN_1969	19
#define ELLIPSOID_WGS_60				20
#define ELLIPSOID_WGS_66				21
#define ELLIPSOID_WGS_72				22
#define ELLIPSOID_WGS_84				23
#define ELLIPSOID_USER_DEFINED			24	///< from user defined value
#define _ELLIPSOID_MAX_					25

#include <zfs/zfsapi.h>

#include <string>

#include "gpstype.h"

namespace zfs {
    namespace math {
        namespace gps {

            /**
            * @brief Grad in Sekunden in Grad in decimal umwandeln
            */
            double ZFS_API convert_grad2dec(double grad);


            /**
            \brief convert decimal to deg,mininute,seconds
            */
            ZFS_API void dec_2_deg_min_sec(double dec, int& grad, int& min, double& sec);

            ZFS_API void deg_min_sec_2_dec(double& dec, int grad, int min, double sec);

            /**
            *
            */
            ZFS_API bool WGS84(double dLatitude, double dLongitude, double& x, double& y, double& z);


            enum bezugssystem_t { BEZUGSSYSTEM_WGS84, BEZUGSSYSTEM_GRS80 };

            /**
            * @brief converts lat/long to UTM coords.  Equations from USGS Bulletin 1532
            * East Longitudes are positive, West longitudes are negative.
            * North latitudes are positive, South latitudes are negative
            * Lat and Long are in decimal degrees
            * Written by Chuck Gantz- chuck.gantz@globalstar.com
            *
            * @param Lat			the latitude value to convert
            * @param Long			the longitude value to convert
            * @param UTMNorthing	the northing value of the converted UTM coordinate
            * @param UTMEasting	the easting value of the converted UTM coordinate
            * @param UTMZone		the zone value of the converted UTM coordinate
            * @param ReferenceEllipsoid the ellipsoide use for the conversion
            */
            ZFS_API void LLtoUTM(const Coordinate&,
                double& UTMNorthing, double& UTMEasting, char* UTMZoneint, int ReferenceEllipsoid = ELLIPSOID_WGS_84);
            ZFS_API void LLtoUTM(const double Lat, const double Long,
                double& UTMNorthing, double& UTMEasting, char* UTMZoneint, int ReferenceEllipsoid = ELLIPSOID_WGS_84);

            /**
            * @brief Converts UTM coords to lat/long.  Equations from USGS Bulletin 1532
            * East Longitudes are positive, West longitudes are negative.
            * North latitudes are positive, South latitudes are negative
            * Lat and Long are in decimal degrees.
            * Written by Chuck Gantz- chuck.gantz@globalstar.com
            *
            * @param UTMNorthing	the northing value of the UTM coordinate
            * @param UTMEasting	the easting value of the UTM coordinate
            * @param UTMZone		the zone value of the UTM coordinate
            * @param Lat			the converted latitude value
            * @param Long			the converted longitude value
            * @param ReferenceEllipsoid the ellipsoide use for the conversion
            */
            ZFS_API Coordinate UTMtoLL(const double UTMNorthing, const double UTMEasting, const char* UTMZone, int ReferenceEllipsoid = ELLIPSOID_WGS_84);
            ZFS_API void UTMtoLL(const double UTMNorthing, const double UTMEasting, const char* UTMZone,
                double& Lat, double& Long, int ReferenceEllipsoid = ELLIPSOID_WGS_84);

            /**
            * @brief This routine determines the correct UTM letter designator for the given latitude
            * returns 'Z' if latitude is outside the UTM limits of 84N to 80S
            * Written by Chuck Gantz- chuck.gantz@globalstar.com
            *
            * @param Lat the latitude value to get the UTM letter
            * @return the letter for the given latitude
            */
            ZFS_API char UTMLetterDesignator(double Lat);

            /**
            * @brief Calculate the offsets and the distance on basis of the GeoDec coordinates.
            *
            * @param la1 the latitude of the first coordinate
            * @param lo1 the longitude of the first coordinate
            * @param la2 the latitude of the second coordinate
            * @param lo2 the longitude of the second coordinate
            * @param dx the distance of the longitude values
            * @param dy the distance of the latitude values
            *
            * @return the distance in meter
            */
            ZFS_API double calculateGeoDistance(double la1, double lo1, double la2, double lo2, double& dx, double& dy);

            /**
            * @brief Calculate the offsets and the distance on basis of the transformed UTM coordinates.
            *
            * @param la1 the latitude of the first coordinate
            * @param lo1 the longitude of the first coordinate
            * @param la2 the latitude of the second coordinate
            * @param lo2 the longitude of the second coordinate
            * @param dx the distance of the longitude values
            * @param dy the distance of the latitude values
            *
            * @return the distance in meter
            */
            ZFS_API double calculateGeoDistanceWithUTM(double la1, double lo1, double la2, double lo2, double& dx, double& dy);

            /**
            * @brief Calculate the offsets and the distance on basis of the UTM coordinates.
            *
            * @param north1 the latitude of the first coordinate
            * @param east1 the longitude of the first coordinate
            * @param zone1 the zone of the first coordinate
            * @param north2 the latitude of the second coordinate
            * @param east2 the longitude of the second coordinate
            * @param zone2 the zone of the second coordinate
            * @param dx the distance of the easting values
            * @param dy the distance of the northing values
            *
            * @return the distance in meter
            */
            ZFS_API double calculateGeoDistanceWithUTM(double north1, double east1, const char* zone1, double north2, double east2, const char* zone2, double& dx, double& dy);


            /**
            * @brief Funktion zur Berechnung der DHHN 92 Normalh�hen H_n aus den GPS-H�hen h_ell
            */
            //double ZFS_API high2HighNormal( double h_ell, double U ); // ellipsoidische H�he h_ell, Undulation U


            /**
            * @brief Enum values of the DOP value
            */
            enum dop_t
            {
                DOP_POOR = 5,
                DOP_FAIR = 4,
                DOP_MODERATE = 3,
                DOP_GOOD = 2,
                DOP_EXCELENT = 1,
                DOP_IDEAL = 0
            };

            /**
            * @brief Method for the transformation of DOP values to enum values of type dop_t
            *
            * @param dop DOP Wert
            */
            ZFS_API dop_t parseDop(double dop);



            /**
            * @brief Class to hold infomation about ellipsoides.
            * The ellipsoids are used in the metheds LLtoUTM und UTMtoLL for coordinate transformations.
            */
            class ZFS_API Ellipsoid
            {
            public:
                Ellipsoid() { id = 0; ellipsoidName = nullptr; EquatorialRadius = 0.; eccentricitySquared = 0.; }

                Ellipsoid(int Id, const char* name, double radius, double ecc)
                {
                    id = Id; ellipsoidName = name;
                    EquatorialRadius = radius; eccentricitySquared = ecc;
                }

                int id;					///< ELLIPSOID_...
                const char* ellipsoidName;	///< name
                double EquatorialRadius;	///< halbachse
                double eccentricitySquared;

            };

            /**
            * @brief List of ellipsoids
            */
            ZFS_API extern Ellipsoid ellipsoid[];

            ZFS_API Ellipsoid* ellipsoidFindById(int id);
            ZFS_API Ellipsoid* ellipsoidFindByName(const std::string& name);



            /**
            * @brief UTC String in Sekunden umrechnen
            */
            ZFS_API double utcstr2sec(const std::string& s);



            /**
            * @brief Get the GPS limits for maximal age of a GPS coordinate before it expires, satelites and HDOP. Check this to decide if a
            * coordinate should be used or not.
            *
            * @return a GPSCoordinate filled in with the limit values
            */
            ZFS_API GPSCoordinate getGPSLimits();

            /**
            * @brief Set the GPS limits for maximal age of a GPS coordinate before it expires, satelites and HDOP.
            *
            * @param limitCoord GPSCoordinate filled in with the limit values
            */
            ZFS_API void setGPSLimits(const GPSCoordinate& limitCoord);

            /**
            * @brief Parse the flags characters and return a string representation
            *
            * @param flags typical 2 characters direct from the gps message pubx feld navstat
            */
            ZFS_API std::wstring gpsFlags2String(const char* flags);

            /**
            * @brief Parse the flags and return true if gps has a fix
            *
            * @param flags typical 2 characters direct from the gps message pubx feld navstat
            */
            ZFS_API bool gpsHasFix(const char* flags);

        }// end namespace gps
    } // end namespace math
} // end namespace zfs