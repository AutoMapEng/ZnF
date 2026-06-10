#pragma once

#include "../zfnav_api.h"
#include "../sens.h"

#include "opencv2/core/core.hpp"


#include "zfQuaternion.h"


#include <iostream>
#include <fstream>

#ifndef NAVLOG
# define NAVLOG 0 // enable nav log
#endif

namespace navsens
{
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
class ZFNAV_API zfTracking
{
public:
	navsens::zfQuaternion qt;   // quaternion
    cv::Mat iError;             // integration error
    cv::Mat dError;             // drift error
    cv::Mat acc;                // accelerlation
    cv::Mat gyr;                // gyroscope

	//Kalman filters Variables
	cv::Mat state;
		
	cv::Mat H;
	cv::Mat R;
	cv::Mat Q;
	cv::Mat P;
	cv::Mat estimates;

	float dt{ 0.1f };
	cv::Mat baroAltAcc;
	float Kp{ 0.5f };
	float Ki{ 0.5f };

public:
    zfTracking();
    ~zfTracking();

	void initOrientation(float,float,float);
	void setCurrentData(float,cv::Mat,cv::Mat, float); // time. cv::Mat[acc. gyr.], baro
	void setCurrentData(float,float,float,float,float,float,float,float);
    void setCurrentData(float,navsens::Sensors);
	void normalization();
	void driftCorrection();
	void systemUpdate();
    void positionEstimation(float zupt);
	void HeigtEstimation();
    void runAlgo(float);
    cv::Mat getAccInNavigationFrames(void);
    void writeMatToFile(cv::Mat& m, const char* filename); ///< for debugging to write a txt

};
#pragma warning(pop)
}
