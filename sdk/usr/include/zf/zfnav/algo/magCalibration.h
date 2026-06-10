#pragma once

#include "../zfnav_api.h"

#include "opencv2/core/core.hpp"

#include <string>

namespace navsens
{

	/**
	* @brief Magnetic Calibration calss
	*/
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
	class ZFNAV_API MagneticCalibration
	{
	public:
		cv::Mat rotR1; // Anti clockwise
		cv::Mat rotR2; // clockwise
		cv::Mat HardIron; // Hard Iron effect
		float softIronFactor{1.f};

	public:
		MagneticCalibration(void);

		cv::Mat calibrate(cv::Mat, cv::Mat);
		cv::Mat removeMotorEffectAlongSingleAxis(cv::Mat, cv::Mat);
	};
#pragma warning(pop)
}
