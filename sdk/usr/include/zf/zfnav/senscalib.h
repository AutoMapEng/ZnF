#pragma once

#include "zfnav_api.h"

#include "../Common/inttypes_.h"
#include "algo/zfQuaternion.h"
#include "algo/magCalibration.h"
#include "algo/accCalibration.h"
#include "algo/gyroCalibration.h"
#include <opencv2/core/core.hpp>

namespace navsens
{

/**
 * @brief Die Kalibrierwerte
 */
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
class ZFNAV_API SensorsCalib
{
public:
    cv::Mat m_accInNavSensCoord;// = cv::Mat(3,3,CV_32FC1);///< transformation matrix for Beschleunigungssensors relative zum Sensorrmittelpunkt
    cv::Mat m_gyroInNavSensCoord;// = cv::Mat(3,3,CV_32FC1);///< transformation matrix for Gyrosensors relative zum Sensorrmittelpunkt
    cv::Mat m_magInNavSensCoord;// = cv::Mat(3,3,CV_32FC1);///< transformation matrix for Manetischsensors relative zum Sensorrmittelpunkt
    cv::Mat m_baroInNavSensCoord;// = cv::Mat(3,3,CV_32FC1);///< transformation matrix for Barometer relative zum Sensorrmittelpunkt
    cv::Mat m_gpsInSensCoord;// = cv::Mat(3,3,CV_32FC1);///< transformation matrix for GPS relative zum Sensorrmittelpunkt
    cv::Mat m_NavSensCoord2ScannerCoord;// = cv::Mat(3,3,CV_32FC1);    ///< transformation matrix for Navsense coordinates to Sensorrmittelpunkt


    // other parameters fixed with sensors eg. noises, offsets, sensivities, biases and etc

    // Calibration objects
	navsens::acceleromterCalibration m_AccCalib;
	navsens::gyroCalibration m_GyrCalib;
	navsens::MagneticCalibration m_MagCalib;

	// Barometer
    double m_baroRefHigh{};
    double m_baroRefPressure{};


    // Factors
    // float boardUnitFactor; // may be for future reference

    SensorsCalib();

    static SensorsCalib *instance;
    static SensorsCalib& getInstance();

    cv::Mat A2BMatrix(float,float, float); // is used to measure a rotation matrix from Frame A to Frame B.
    void accToScannerCoordinates(cv::Mat&);
    void gyroToScannerCoordinates(cv::Mat&);
    void magToScannerCoordinates(cv::Mat&);
    void baroToScannerCoordinates(cv::Mat&);
    void gpsToScannerCoordinates(cv::Mat&);

};
#pragma warning(pop)


}//end namespace navsens
