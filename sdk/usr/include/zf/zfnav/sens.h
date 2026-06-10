#pragma once

#include <map>
#include "../Common/inttypes_.h"


namespace navsens
{


    /**
     * @brief The NavSensors class für die Uebergabe/speicherung der Sensorwerte
     */
    #pragma pack(1)
    class Sensors
    {
    public:
        /// Bitmaske welche Werte ausgelesen wurden bzw vorhanden sind
        ///
        /// Heißt: Bit wird gesetzt, wenn der Wert im aktuellen Zyklus gelesen
        /// wurde.
        /// Hat der Sensor kein Update im aktuellem Zyklus empfangen, ist das
        /// Flag nicht gesetzt
        uint32_t items{};

        enum sens_t
        {
            NONE        = 0,
            HORZ        = (1<<0),	///< horizontal position
            LEVEL       = (1<<1),
            GPS         = (1<<2),
            BARO        = (1<<3),
            TEMP        = (1<<4),
            MAG         = (1<<5),
            ACC         = (1<<6),
            GYRO        = (1<<7),
            TIME_PC     = (1<<8),
            TIME_SENS   = (1<<9),
            ROTATION     = (1<<10),
            TRANSLATION  = (1<<11),
            IMUTILT  = (1<<12),
            IMUA  = (1<<13),  ///< IMU improved values for Trimble X12 and ZF 5016A
        };

        enum {
            TRACK_UNDEFINED,
            TRACK_OUTDOOR,
            TRACK_INDOOR
        };

        enum { GPS_FLAGS_SIZE = 4 };


        // Plausibility tests
        // They check whether the sensor does send values at all
        //
        // Theroy is:
        // It's extremely unlikely that all axis show 0. This is only at startup
        // the case. If Value isn't updated in current cycle, the fields carry
        // the old values from cycles before.

        bool is_baro_valid() const noexcept
        {
            return baro != 0.0f || temp != 0.0f;
        }

        bool is_temp_valid() const noexcept
        {
            return is_baro_valid();
        }

        bool is_accel_valid() const noexcept
        {
            return accX != 0.f || accY != 0.f || accZ != 0.f;
        }

        bool is_gyro_valid() const noexcept
        {
            return gyroX != 0.f || gyroY != 0.f || gyroZ != 0.f;
        }

        bool is_mag_valid() const noexcept
        {
            return magX != 0.f || magY != 0.f || magZ != 0.f;
        }

        bool is_gps_valid() const noexcept
        {
            return gpsFlags[0] || gpsHDop != 0.f || gpsVDop != 0.f;
        }

        //=== Roh Werte ===

        // Time
        uint32_t timeSens{};   ///< die Zeit die im Sensorboard erzeugt wurde [ms]  Ueberlauf nach ca 49Tagen
        uint32_t timePC{};     ///< die locale Zeit des PC wann das letzte mal nach Werten geschaut wurde [ms]

        // Horizontal and Vertical
        float horz{};         ///< [deg]
        float vertical{};     ///< [deg]
        uint32_t horzLastread{}; // [ms]

        // Libelle
        float levelX{}, levelY{}; ///< [deg]
        uint32_t levelLastread{}; // [ms]

        // IMU Tilt
        float tiltX{}, tiltY{}, tiltAng{}; ///< [deg]

        // GPS
        double gpsLat{}, gpsLong{}, gpsAlt{}; ///< Position und Höhe
        int32_t gpsSatelit{};             ///< Anzahl Sateliten
        float gpsHDop{};                  ///< aktuell hdop = horizontal
        float gpsVDop{};					///< aktuell vdop = hoehe
        float gpsHacc{};					///< accuracy [meter]
        float gpsVacc{};					///< accuracy [meter]
        char gpsFlags[GPS_FLAGS_SIZE]{};	///< characters null terminated string
        uint32_t gpsUTC{};                ///< utc Teit in sekunden der position
        uint32_t gpsLastread{};           ///< time of last gps-fix [ms]
        uint32_t gpsAge{};                ///< last fix age

        // Barometer
        float baro{};                    ///< [bar]

        // Temperatur
        float temp{};    ///< [C]

        // Magnetometer
        float magX{}, magY{}, magZ{};        ///<  (HMC5843) [gauss]
        float magTs{};                   ///< timestamps [s]

        // Accelerometer
        float accX{}, accY{}, accZ{};        ///< Beschleunigung (SCA3100-D07) [g]
        float accTs{};                   ///< Timestamps [s]

        // Gyroscope
        float gyroX{}, gyroY{}, gyroZ{};     ///< Gyroskop (L3G4200D) [deg per sec]
        float gyroTs{};                  ///< Timestamp [s]

        //=== Berechnete Werte ===

        // Track
        uint32_t track{ TRACK_UNDEFINED };             ///< information about track was generated
        float tx{},ty{},tz{};            ///< Translation [meter]
        float  rx{},ry{},rz{}, rz_rel{};   ///< Rotation [rad]
        float txs{},tys{},tzs{};			///< Translation Standard Deviation.[meter]
        float  rxs{},rys{},rzs{};			///< Rotation Standard Deviation.[rad]
        bool updown{};                ///< True is scanner upside-down
        bool trackMode{};             ///< True für indoor oder False für outdoor
        float refHeight{};            ///< refernce height...... ???
        float zupt{};                 ///< is scanner moving
        float mapResolution{};        ///< Map resolution
        float compassNorth{};        ///< Compass North for scans TODO: for tracks

        bool libelleInWorkingRange{}; ///< true to turn on level and false to turn off

        Sensors() = default;

        void clr()
        {
            *this = Sensors();
        }

    };
    #pragma pack()


    //#############################################################################################################################


#pragma pack(1)
    /**
    * \brief a varable list of sensors like Sensors
    */
    class SensorList
    {
    public:
        // !!! do not change id or order, just add items !!!
        enum sens_t : uint32_t
        {
            TIME_SENSOR = 1,	///< time from the hardware
            TIME_PC,		///< time added later on
            TEMPERATURE,	///< tempperature of the device
            UPSIDEDOWN,		///<
            HARDWARE, ///< 1=scanner, 2=standalone-naviboard
            HARDWARE_VERSION, ///< a hardware id depends of sensors mounted
            SOFTWARE_VERSION, ///< a software id

            SCANNER_STATUS_BYTE = 10,
            NAVI_MODE, ///< how things are calculated: tracking, scanning, image, ...
            SEQ_NUM,   ///< number of processes frame

            // Accelerometer
            ACC_X = 100,
            ACC_Y,
            ACC_Z,
            ACC_TIME,

            // Magnetometer
            MAG_X = 200,
            MAG_Y,
            MAG_Z,
            MAG_TIME,

            // Gyroscope
            GYRO_X = 300,
            GYRO_Y,
            GYRO_Z,
            GYRO_TIME,

            // Barometer
            BARO = 400,
            BARO_TIME,

            // GNSS
            GPS_SAT = 500,
            GPS_LAT,
            GPS_LONG,
            GPS_ALT,
            GPS_HDOP,
            GPS_VDOP,
            GPS_HACC,
            GPS_VACC,
            GPS_TIME, // gps time of the fix
            GPS_FIX,

            // leveling system / tilt
            LEVEL_X = 600,
            LEVEL_Y,
            LEVEL_TIME,
            LEVEL_STATUS,

            // motor angle
            MOTOR_VERT = 700,
            MOTOR_HORZ,

            // calculated value (ahrs)
            ROT_X = 800,
            ROT_Y,
            ROT_Z,
            ROT_REL,
            ROT_TIME,

            // calculated value (tracking)
            TRANS_X = 900,
            TRANS_Y,
            TRANS_Z,
            TRANS_TIME,

            //
            IMU_TILT_X = 1000,
            IMU_TILT_Y,
            IMU_TILT_Z,

            //
            CAM_cnt = 1200,

        };

        std::map< sens_t, double>  lst; ///< list of id + value

        /// check if the item with id is in the list
        bool has(sens_t i) const { return lst.find(i) != lst.end(); }

        /// get the value of the item or the default if it does not exist
        double getValue(sens_t id, double def = 0.) const { auto it = lst.find(id); return it == lst.end() ? def : it->second; }
        float getValueFloat(sens_t id, double def = 0.) const { auto it = lst.find(id); return it == lst.end() ? static_cast<float>(def) : static_cast<float>(it->second); }
    };
#pragma pack()



}//end namespace
