#pragma once
// schlechte implementierung da opencv benutzt in der include. Sollte standalone sein. 

/*
* by F. Shahzad - 13.02.2014
*/


#include "../zfnav_api.h"

#include <iostream>
#include <opencv2/core/core.hpp>

namespace navsens
{
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
    class ZFNAV_API zfQuaternion
    {
    private:
        cv::Mat q;
    public:
        // Constructors
        zfQuaternion( void );
        zfQuaternion( float, float, float );
        zfQuaternion( float, float, float, float );
        zfQuaternion( cv::Mat );
        ~zfQuaternion( void );

        // Operator overloading
        zfQuaternion operator+( const zfQuaternion& );
        zfQuaternion operator+=( const zfQuaternion& );
        zfQuaternion operator*( const zfQuaternion& );
        zfQuaternion operator*( const float );
        zfQuaternion operator/( const float );
        zfQuaternion operator~();

        // Setters
        void setXYZ( float, float, float );
        void setX( float );
        void setY( float );
        void setZ( float );
        void setR( float );

        // getters
        float getX();
        float getY();
        float getZ();
        float getR();

        // auxilary functions
        float norm(); // calculate the norm

        // Conversions
        cv::Mat toDCM();
        cv::Mat toEuler();
        cv::Mat toQuaternion();
        void DCM2Quaternion( cv::Mat );
        cv::Mat euler2DCM( float, float, float );
        void euler2Quaternion( float, float, float );
    };
#pragma warning(pop)
}
