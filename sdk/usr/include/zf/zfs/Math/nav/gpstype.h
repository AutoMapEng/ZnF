#pragma once

#include <zfs/zfsapi.h>

#include <cmath>

namespace zfs 
{
    namespace math
    {
        namespace gps
        {
            /**
            * @brief Helper class to store GPS Coordinates
            */
            class ZFS_API Coordinate
            {
            public:

                double latitude = 0.;    ///< gps
                double longitude = 0.;   ///< gps 
                double altitude = 0;    ///< not set all the time [meter]

                Coordinate() {}
                Coordinate(double la, double lo, double a = 0.) : latitude(la), longitude(lo), altitude(a) { }

                /// is coordinate set or empty
                bool empty() const {
                    return !(std::abs(latitude) > 0.000001 ||
                        std::abs(longitude) > 0.000001);
                }
            };

            //..............................................................................

            /**
            * @brief Helper class to store GPS Coordinates and quality values.
            */
            class ZFS_API GPSCoordinate : public Coordinate
            {
            public:

                double satelites = 0;
                double hdop = 100.;
                double vdop = 100.;
                double hacc = 0.;
                double vacc = 0.;
                char flags[4]{};

                double age = 0.;

                bool use = true;

                GPSCoordinate() {}
                GPSCoordinate(double la, double lo, double a = 0.) : Coordinate(la, lo, a) {}
            };



        }// end namespace gps
    } // end namespace math
} // end namespace zfs