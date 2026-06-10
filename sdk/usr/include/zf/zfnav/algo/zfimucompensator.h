#pragma once
/*
* by F. Shahzad - 18.09.2018
*/

#include "../zfnav_api.h"
#include "../sens.h"


namespace navsens
{
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle

class ZFNAV_API zfGuassian
{
private:
    float mean;
    float sigma;

public:
    zfGuassian();
    zfGuassian(float mean_, float sigma_);
    ~zfGuassian();

    zfGuassian operator+( const zfGuassian& );
    zfGuassian operator*( const zfGuassian& );


    // Getters
    float getMean() const;
    float getSigma() const;

};


class ZFNAV_API zfIMUCompensator
{
private:
    float currentAngle;
    float oldTime;
    float dt;
    float modelX, modelY;
    float sensorX, sensorY;
    float kWeight;
    float accSigmaX;
    float accSigmaY;
    float gyroSigmaX;
    float gyroSigmaY;
    float offSetX;
    float offSetY;

    Sensors currentIMUValues; // use calcibrated values

public:
    zfIMUCompensator();
    zfIMUCompensator(bool isKalman);
    ~zfIMUCompensator();

    void setCurrentData(navsens::Sensors sens, bool upsideDown);
    void calibrate();
    void getCalibration();
    void getModelTilt();
    void getSensorTilt();
    void update();

    float tiltX, tiltY;
    float angle;

    bool initialized;
    uint32_t tilt;             ///< information about track was generated

    uint8_t algType;

    enum
    {
        TILT_UNDEFINED,
        TRACK_KALMAN,
        TRACK_HG
    };
};

#pragma warning(pop)

}
