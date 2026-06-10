#pragma once

#include "../zfnav_api.h"
#include "../sens.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <functional>
#include <atomic>

#include <boost/circular_buffer.hpp>
#include <boost/circular_buffer_fwd.hpp>


#include <opencv2/core/core.hpp>
// needed for cv::fitellipse function for magnetic calibration

// extrem schlecht hier
#include <QDateTime>

#include "zfQuaternion.h"
#include "zfTracking.h"
#include "AHRS.h"
#include "zfimucompensator.h"



//TODO group to generel, motion-detection and level/tracking-system
//TODO calib in separate block

namespace navsens
{


class StreamOut;


/**
* @brief Tracking von eriner scanposition zur naechsten
*
*/
class ZFNAV_API Tracking
{

public:
    /**
     * @brief Erzeugt Tracking object mit internem Ring-Buffer
     *
     * @note implementiert einen Ring-Buffer. Es können also maximal
     * capacity-Elemente gespeichert werden.
     *
     * @param capacity Größe des Ringbuffers. (>= 0) [n]
     */
    Tracking(/*int capacity = 0*/);
    ~Tracking();

    // static Tracking* instance;
    static Tracking& getInstance();

    // Supported Devices
    enum device_t
    {
        DEVICE_UNSUPPORTED, // DEFAULT
        DEVICE_5010X,   // 5010X
        DEVICE_5016, // 5016
        DEVICE_5016A, // starting with 5016A (use IMU tilt)
        DEVICE_5024E, // Only Tilt
        DEVICE_5024A, // Tilt + compass + Trajectory + GNSS
        DEVICE_5024S, // Tilt + compass + Trajectory + GNSS
        DEVICE_TRIMBLE // oem (relative IMU tilt)
    };

    /// set the oem / device version like 5016 or 5016A. Has to be set before scan starts
    void setDevice(device_t);
    static std::string deviceNameAsString(device_t);

    // Error codes
    enum err_t
    {
        ERR_OK,
        ERR_ARRAY_EMPTY,
        ERR_EXCEPTION
    };

    // Status Codes
    enum status_code_t {
        STOPPED,
        START, // beim startup des systems
        SCAN,
        MOVE,
        ERROR, // currently not used
        CAMERA, // currently not used
        CALIB,
        PROFILER
    };
    status_code_t Status() const { return m_status; }

    // movement detection reason of movement
    enum moved_reason_t {
        MOVED_NO,
        MOVED_LEVEL, ///< level change detected (works all the time)
        MOVED_IMU,  ///< imu tilt detected (only after the scan has finished and while capturing images)
        MOVED_TRACK, ///< translation detected
        MOVED_INITIAL  ///< inital at statup to tell that the status is unknown
    };

    // config values for movement detection
    enum moved_configuration_t {
        CFG_MOVE_LEVEL=1,
        CFG_MOVE_TILT=2,
        CFG_MOVE_TRACK=4,
        CFG_MOVE_DEFAULT = /*CFG_MOVE_LEVEL | CFG_MOVE_TILT | */CFG_MOVE_TRACK ,
        CFG_MOVE_ALL=0xff
    };

    /** config the movement detection by setting bitmask of cfg_mov_t
    *   tells whether level, track or imu based check should be activated
    *   must be set before the scan starts
    */
    struct movement_detection_configs_t
    {
        int detection{ CFG_MOVE_DEFAULT };
        double level_limit{ 0.075 };
        double imu_limit{ 0.5 };
        double track_limit{ 0.5 };

    };
    void setMovementDetectionConfigs(const movement_detection_configs_t& cfgs)
        { m_movement_configs = cfgs; }
    const movement_detection_configs_t& getMovementDetectionConfigs() const
        { return m_movement_configs; }

    void enableMovementDetection(bool en = true) { m_isMovementDetectionEnabled = en;}

    /**
     * @brief Test whether the scanner has detected movement
     *
     * The returned value is the type of the detected movement, while
     * MOVED_NO is effictively 0 (false).
     *
     * @return Detected movement reason
     */
    moved_reason_t hasScannerMoved() const;

    /**
     * @brief Reset any detected movement, effectively marking the scanner as
     * not moved
     *
     * @post hasScannerMoved() returns moved_t::MOVED_NO
     */
    void resetMoved();
    /// moved_t as a string
    static std::string movementReasonAsString(moved_reason_t);


    /** Scanner Logging Paramters/Files
    */
    struct navigation_logging_parameters_t
    {
        int lockTime{}; ///< navigation lock time in minutes
        bool scanLog{};
        bool trackLog{};
        std::string m_lockFile = "/mnt/a/scans/navpos.nlk" ; ///< nlk is navi lock
    };
    void setTrackingLogParameters(const navigation_logging_parameters_t& prm)
        { m_logging_parameters = prm; }
    const navigation_logging_parameters_t& getTrackingLogParameters() const
        { return m_logging_parameters; }
    bool StartLog( const std::string& file );
    void StopLog();

    /** Tracking/Navigation Configurations and IMU Calibrations
    */
    struct naviboard_calibration_parameters_t
    {
        double ca1{0.}, ca2{0.}, ca3{0.}, ca4{0.}, ca5{0.}, ca6{0.};
        double cg1{0.}, cg2{0.}, cg3{0.}, cg4{0.}, cg5{0.}, cg6{0.};
        double cm1{0.}, cm2{0.};
        double imuOffsetX{0.};
        double imuOffsetY{0.};
        QDate calibdate{QDate::currentDate()};
        std::string m_calibFile = "/mnt/a/scans/naviboard.calib"; // calibration logs
    };
    void setNaviboardCalibrationParamters(const naviboard_calibration_parameters_t& cfg)
        { m_calibration_parameters = cfg; }
    const naviboard_calibration_parameters_t& getNaviboardCalibrationParamters() const
        { return m_calibration_parameters; }

    // Graphics and Webviews
    bool showGPSViews() const;
    bool showTrackingViews() const;

    // naviboard calibration and related routines.
    void startCalibration();
    void calculateCalibration(const bool useOld, const bool doFullCalib);
    bool isCalibrated() const;
    bool applyCalibration();
    void fullCalibration(cv::Mat& data, cv::Mat& mean, cv::Mat& std);

    /** Tracking/Navigation Configurations and IMU Calibrations
    */
    struct navigation_general_configs_t
    {
        // default value for 5010
        double iEllipK1 {0.0115};
        double iEllipK2 {0};
        double iEllipK3 {0.00675};

        double detectLowerLimitWalk{1.2};
        double detectUpperLimitWalk{1.75};
        double detectLowerLimitDolly{1.10};
        double detectUpperLimitDolly{1.75};

        double maxPrecisionError{19.4f};    ///< in m
        double minPrecisionError{5.1f};   ///< in m
        double minRotationError{15};     ///< in m

        double compassOffsetFactor = 270; //lrcserver::Conf("nav_compass_offset",180.); // compass offset
        double compassOffsetFactor2 = 90;
        double cameraOffset {-90.0};

        double dollyOffset{180.}; ///< angular offsets to adjust motor encoder/libelle during dolly mode
        double lastPosSearchRadius{0.5};
        bool useAdoptiveDistance{true};
    };
    void setScannerConfiguration(const navigation_general_configs_t& cfg)
        { m_navigation_configs = cfg; }
    const navigation_general_configs_t& getScannerConfiguration() const
        { return m_navigation_configs; }

    /**
     * Functions/Callbacks to fetch Motor/Horz/level data from Scanner
    */

    typedef void (*cbGetFurtherSensor_f)(Sensors&);
    cbGetFurtherSensor_f m_cbGetFurtherSensorsMotor; // callback um encoder werte aus Motor einzulesen.
    cbGetFurtherSensor_f m_cbGetFurtherSensorsBuffer; // callback um encoder werte aus Buffer einzulesen.

    void SetGetSensorFromMotorCB( cbGetFurtherSensor_f cb) {m_cbGetFurtherSensorsMotor=cb;}
    void SetGetSensorFromBufferCB( cbGetFurtherSensor_f cb) {m_cbGetFurtherSensorsBuffer=cb;}

    // callback für Calculate Values e.g. Level In Working Rang am ende des threads
    typedef void (*cbCalculated_f)(Sensors&);
    cbCalculated_f m_cbCalculated;
    void SetCalculatedCB( cbCalculated_f cb) {m_cbCalculated=cb;}

    /** Functions/Callbacks for logs/trace
    */
    typedef std::function< void(const std::string&) > cbTrace_f;
    cbTrace_f m_cbTrace; // callback for trace
    void SetTraceCB(cbTrace_f cb) { m_cbTrace = cb; }

    typedef std::function< void(const std::string&) > cbLog_f;
    cbLog_f m_cbLog{}; // callback for trace
    void SetLogCB(cbLog_f cb) { m_cbLog = cb; }

    /// Information about Navigation Board
    /** Check if navigation is enabled - Should be use before giving out navigation results
    */
    void enableNavigation(bool en = true) { m_isNaviEnabled = en; }
    std::string naviboardHWRevision() const;
    std::string naviboardHWSerial() const;
    std::string naviboardFirmware() const;
    bool isNaviboardConnected() const;
    bool isNaviboardWorking() const;
    void startNewProject() { delta_angle = 0; m_orientation= 0;} // Reset Navigation for New Project

    /** Functions to start/Stop/move the navigation
     */
    // start scan navigation status
    bool StartScan( const std::string& slog="");   // waehrend des scannings werte aufnehmen fuer gps,compass,barometer
    bool StartScan( bool asProfiler=false, const std::string& slog="");   // waehrend des scannings werte aufnehmen fuer gps,compass,barometer
    bool StartScan( bool asProfiler, bool useCompensator, const std::string& slog);
    bool StartMovement( const std::string& slog="");   // nach dem scanning, bewegung aufzeichnen
    void Stop( );
    void Stop(status_code_t st); // stop navigation with status switch

    /*
     * Detectors/tracking navigation status
     */

    // return True if Scanner is being carried in Hand
    float isWalking(const float T) const;

    // return True if Scanner is being Placed on Tripid
    float isLifted(const float T) const;

    // return True if Scanner has lost its last recorded Position
    bool isLastPositionChanged() const;

    // return True if first scan in a Project
    bool isFirstScanInProject() const;

    // return True if Scanner is Rotating (valid for 2 secs)
    bool isRotating();

    // return True if Scanner is Rotating (old lagacy code for 5010x)
    bool isRotatingLegacy();

    bool getHeigth();
    void resetRefHeight();
    bool getCompass();
    bool getNavigation();
    bool getNavigationWithDolly();
    bool getNavigationWithoutDolly();
    bool getNavigationDurringScan();

    bool GetLastReading(navsens::Sensors& sens );
    bool GetNavResults(navsens::Sensors& sens );

    bool postProccessScan();
    bool postProccessScanZF(); // Legacy code removed after Rev. 34951
    bool postProccessScanTrimble();

    // toggle dolly/walk
    bool toggleNaviMode();

    void clearqueue();
    void updateLibelleStatus();
    void updateNaviboardStatus();
    void updateHardwareStatus(bool st) { m_checkHardware = st; }
    void restartMovementDetection();

    int RecN() const; // anzahl aufgenommener records
    void StartThread();
    void StopThread();
    void backgroundCalculation(Sensors& s);
    void Add(Sensors& s);
    void Reset(); // reset the buffer
    void setStatus(status_code_t st);
    std::string statusString() const;
    std::string statusString(status_code_t) const;

    /// the time of the sensors [sec]
    double sensorTime() const { return m_sensorTime + m_sensorTimeOverflows; }

    // return status if level in allowed woring range
    bool lastPosSearchRadius() const { return m_navigation_configs.lastPosSearchRadius; }

    // return status if level in allowed woring range
    bool levelInWorkingRange() const { return m_levelInWorkingRange; }

    /// upside down flag based on imu. Works all the time
    bool isDownside() const { return m_isScnnerDownSide; }

    // true if the result is available
    bool isPostProcessed() const { return m_postProcessed; }

    // true if the result is available
    bool isIMUProcessed() const { return m_postIMU; }

    // true if the imu is ready for movement
    bool isIMUReadyForMovement() const { return m_postIMUReadyForMovement; }

    // True if Indoor Track is used (scanner is indoor because GPS data is not reliable)
    bool isIndoorTrack() const;

    // read last saved sanner position from lock file return true for a valid position
    bool isLastPositionAvialble(void);

    // read last saved sanner position from lock file return true for a valid position, also extract x,y,yaw
    bool isLastPositionAvialble(float& x, float& y, float& yaw);

    /// Debugging
    void trace(const std::string&);
    void log(const std::string&);
    std::string DumpSensors( const Sensors& s) const;
    std::string DumpNavigation( const std::string& cr="\n" ) const; // for nav -dump or similar

    bool publishIMUTiltValues(double& x, double& y);


    /*
     * UPDATE Navigation from External Source
     * Available Via nav -debug and nav -setnav command
    */
    void updateNavigationValues(double tx, double ty, double tz, double rx, double ry, double rz);  // set navigation parameter from outside only in debug mode
    void updateNavigationValues(double tx, double ty, double tz, double qx, double qy, double qz, double qr); // with quaternions

    void setNavigationDebugMode(bool en)
    {
        debugNavigation = en;
        m_resultMovement.clr();

        if (debugNavigation == false)
        {
            m_resultMovement.tx = m_resultScan.tx;
            m_resultMovement.ty = m_resultScan.ty;
            m_resultMovement.tz = m_resultScan.tz;
            m_resultMovement.items |= navsens::Sensors::TRANSLATION;

            m_resultMovement.rx = m_resultScan.rx;
            m_resultMovement.ry = m_resultScan.ry;
            m_resultMovement.rz = m_resultScan.rz;
            m_resultMovement.items |= navsens::Sensors::ROTATION;

            m_imagerTracker.initOrientation(0, 0, m_resultScan.rz);
        }
    }
    bool getNavigationDebugMode(void) const {return debugNavigation;}

    // Streaming and Output
    /**
     * @brief Set a Streamer
     *
     * If the streamer is connected, the tracking thread will start to call
     * send_frame() on it.
     *
     * @param streamer Streamer to set. Use nullptr to disable streaming
     */
#ifndef _WINDOWS
    void SetStream(std::unique_ptr<StreamOut> streamer);
#endif
    cv::Mat plotting(int wd, int hg, int type);


    Sensors m_resultScan;
    bool nav_usedolly{};
    bool track_reset{};
protected:
    // Algorithms
    zfIMUCompensator m_imuLibelle;
    zfTracking m_imagerTracker;

    // Double Check
    float angle_offset{};
    float tiltX{}, tiltY{}, tiltAng{};

    // TODO: REMOVE THEM
    double imuOffsetX{},imuOffSetY{};  ///< Tilt Estimation - Scan Center values


protected:

    std::unique_ptr<boost::circular_buffer<navsens::Sensors>> m_sensorvalues;  ///< Ringpuffer mit aufgenommenen Datenwerten
    Sensors m_LastReading;      // fuer interne Berechnung
    Sensors m_LastRead;         // Fuer die Abfrage von ausserhalb
    Sensors m_resultMovement;   // fuer interne Berechnung
    Sensors m_resultMove;       // Fuer die Abfrage von ausserhalb
    Sensors m_PrevReading;

    std::atomic<double> m_sensorTime{};        // the time of the sensors
    std::atomic<double> m_sensorTimeOverflows{};

    float txOld{}, tyOld{}, tzOld{};
    float rz_old{};
    float scan_horz_offset = 0.0;
    float selection_scan_horz_offset = 180.0;
    double m_time_Scan_start_sec{}; ///< captured time scan started

    moved_reason_t scannerMovedReason{ MOVED_INITIAL };

    bool m_levelInWorkingRange{}; ///< will be checkt with every sensorvalue comming in
    bool m_isScnnerDownSide{};
    bool naviboard_status{true};  ///< record the status from plausibility test: wether the naviboard is working on not.

    void backgroundThread();
    volatile bool m_stop{};   ///< thread stoppen
    status_code_t m_status{ STOPPED };        ///< aktuell
    status_code_t m_last_status{ STOPPED };
    double m_status_changed_time{};

    device_t m_currentDevice{ DEVICE_5016 };

    // Stream/Write out calculated data
#ifndef _WINDOWS
    std::unique_ptr<StreamOut> m_streamer;
#endif

    bool m_isRotating{}; ///< flag is horz rotating is detected. Flag will hold true for 2seconds after still
    bool m_postProcessed{};
    bool m_postIMU=false;
    bool m_postIMUReadyForMovement = true;
    double m_orientation{};

    // logging and file related stuff
    FILE* m_fp{}; ///< record sensors
    FILE* m_fp_debug{}; ///<
    bool m_log{};
    bool m_lock{};
    std::string m_logFile;

    // Protected Functions
    //====================
    void movementDetectionUsingLevels();
    void movementDetectionUsingIMU();
    void movementDetectionUsingTrack();
    /**
     * @brief dumpNavigationLock Dump current navigation state
     * @param useState If true then last position (tx[m], ty[m], rz[m] are also saved)
     */
    void dumpNavigationLock(bool useState);
    /**
     * @brief extractNaviLock Extract navi lock as saved by dumpNaviLock function.
     * @param tx returns last locked Tx [m]
     * @param ty returns last locked Ty [m]
     * @param rz returns last locked Rx [deg]
     * @return true if navilock is valid
     */
    bool extractNavigationLock(float& tx, float& ty, float& rz);

    //    void trackEstimation(cv::Mat, cv::Mat, cv::Mat,cv::Mat,cv::Mat,cv::Mat);
    void postScanEstimations(cv::Mat, cv::Mat, cv::Mat,cv::Mat,cv::Mat,cv::Mat, bool);
    cv::Mat heightEstimation(cv::Mat,cv::Mat,cv::Mat);
    void prepareOutputParameters();

    void detectRotating(const Sensors& sens);
   
 private:

    void doStart();
    void doMove();
    void doCalib();

    /**
     * @brief ProccessMovement function to handle all the movement processing
     * @return
     */
    bool proccessMovement();

    void getIMUTilt();
    void captureStartTilt();

    float zuptAnalysis() const;

    void switchOffMovementDetection();

    navigation_logging_parameters_t m_logging_parameters{};
    navigation_general_configs_t m_navigation_configs{};
    naviboard_calibration_parameters_t m_calibration_parameters{};
    movement_detection_configs_t m_movement_configs{};

protected:
    // Movement Detection Paramters
    bool m_isMovementDetectionEnabled = true;

    // variables for detecting movement based on level
    double level_current_r{};
    double level_start_r{};
    double level_start_horz{};
    double level_time{};
    bool level_movement_start_flag = false;

    // variables for IMU Motion Detection
    double imu_current_r = {};
    double imu_current_x = {};
    double imu_current_y = {};
    double imu_current_ang = {};
    double imu_start_r = {};
    double imu_start_x = {};
    double imu_start_y = {};
    double imu_start_ang = {};
    bool imu_movement_start_flag = false;
    double imu_tilt_time{};

    //variables for Track Motion Detection
    double usePositionOffset = false;
    double translationX = {};
    double translationY = {};
    bool track_movement_start_flag = false;

    // Map dimension
    double mapRes{};
    float gT{}, gTd{}, gRotate{}, gZ{}; // hold global reference to the values
    double oldheight{};

    // zupt and drift analysis
    int zeroCount{}; // for motor rotation
    float motorNorth{};
    float libelle2{};
    float angRange{};   // minimum threshold search radius as angle in degree
    double driftErrorX{}, driftErrorY{};

    // Performanze / Debugging
    int m_runN{};                 ///< anzahl gelesener werte

    //CHECK THESE
    int m_kalmanCounter{}; ///< first 1000 values are ignored to get a stable reading.

    bool m_isNaviEnabled = true; //TODO obsolete   from the scanner configuration to allow to disable motion detection
    bool isLastScanProcessed = false; // TODO This need to be modified at a later stage^^
    bool m_checkHardware{ true };
    bool debugNavigation{}; ///< added flag to debug navigation calculations (mvoement & scanning)

    bool log_enabled{};
    float oldAccTime{};

    float horz_angle_offset{}; ///< keep track of intial relative angle from KF initialization. This need to be subtracted from current rz to get relative value.
    float delta_angle{}; ///< keep track of intial relative angle from KF initialization. This need to be subtracted from current rz to get relative value.
    float horz_angle_encoder{}; ///< Horizontal angle with reference to encoder
    bool isfixed = false;

    float kalmanCounter_limit{};

    float angle_start{};
    float angle_end{};
    bool angle_set = false;
};
}//end namespace
