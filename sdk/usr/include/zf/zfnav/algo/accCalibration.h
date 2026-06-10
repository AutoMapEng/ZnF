#pragma once

#include "../zfnav_api.h"

#include <opencv2/core/core.hpp>

#include <opencv2/imgproc/imgproc.hpp>


namespace navsens
{

	/**
	* @brief Acceleromter Calibration calss
	*/
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
	class ZFNAV_API acceleromterCalibration
	{
	private:
		cv::Mat OffSets ;
		cv::Mat scaleFactors;

	public:
		acceleromterCalibration(void);
		cv::Mat calibrate(cv::Mat,float alpha);
	};
#pragma warning(pop)
}
