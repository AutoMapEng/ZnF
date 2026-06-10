#pragma once
#include "zfnav_api.h"

#include <string>

#include "sens.h"

namespace navsens
{


    ZFNAV_API
    std::string sens2xml( const Sensors& s );

    ZFNAV_API
    std::string sens2str( const Sensors& s, const std::string& cr="" );

    /// os independed get pc time [ms]
    ZFNAV_API
    uint32_t getPcTime();

    /// calculate the age of the sensor value sens [ms]
    ZFNAV_API
    uint32_t SensorAge(const Sensors& sens, Sensors::sens_t ); // Alter des Sensorwertes, vor allem GPS, horz



    //############### variable sensor list ####################

    // get a short name of the id
    ZFNAV_API
    std::string sensID2string(SensorList::sens_t);


    /**
    * \brief create a string
    * \param spacer user " " or "\n" for example
    */
    ZFNAV_API
    std::string sensList2string(const SensorList&, const std::string& spacer);


    /// the opposide of sensList2string
    ZFNAV_API
    void parseSensorList(const std::string& str, SensorList& sens);

    /// convert from SensorList to old Sensors
    ZFNAV_API
    void senslist2sens(const navsens::SensorList& l, navsens::Sensors& s);

}//end namespace
