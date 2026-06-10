#pragma once

#include "../zfnav_api.h"

#include "opencv2/core/core.hpp"


namespace navsens
{

	/**
	* @brief Gyro Calibration calss
	*/
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
	class ZFNAV_API gyroCalibration
	{
	public:
		cv::Mat OffSets ;
		cv::Mat scaleFactors;

	public:
		gyroCalibration();

		void calcCalibration(cv::Mat);
		cv::Mat calibrate(cv::Mat);
		cv::Mat calibrate(cv::Mat,float);
	};
#pragma warning(pop)
}
