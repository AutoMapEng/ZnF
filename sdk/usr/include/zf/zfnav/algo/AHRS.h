#ifndef _AHRS_H
#define _AHRS_H

#include "../zfnav_api.h"

#include "opencv2/core/core.hpp"


#include "zfQuaternion.h"
#include "../sens.h"

namespace navsens
{
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
class ZFNAV_API AHRS
{
public:
	navsens::zfQuaternion qt;   // quaternion
	cv::Mat iError;   // integration error
	cv::Mat dError; // drift error
	cv::Mat acc;    // accelerlation 
	cv::Mat gyr;    // gyroscope 
	cv::Mat mag;    // magnetometer
	
	//Kalman filters Variables
	cv::Mat state;
		
	cv::Mat H;
	cv::Mat R;
	cv::Mat Q;
	cv::Mat P;
	cv::Mat estimates;

	float dt{};
	cv::Mat baroAltAcc;
	float Kp{};
	float Ki{};

public:
	AHRS();
	~AHRS();

	void initOrientation(float,float,float);
	void setCurrentData(float,cv::Mat,cv::Mat,cv::Mat, float); // time. cv::Mat[acc. gyr. mag], baro
	void setCurrentData(float,float,float,float,float,float,float,float,float,float,float);
    void setCurrentData(float,navsens::Sensors);

	void normalization();
	void driftCorrection();
	void systemUpdate();
	void positionEstimation();
	void HeigtEstimation();
	void runAlgo();
};
#pragma warning(pop)
}
#endif //_AHRS_H
