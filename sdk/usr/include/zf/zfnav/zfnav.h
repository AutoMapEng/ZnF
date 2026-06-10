#pragma once

/**
 @brief Anbindung des Navigation Sensorboardes inc Algorithmen für die Auswertung
        Laeuft unter LINUX und WINDOWS
*/

#define ENABLE_ZFNAV
#ifdef  ENABLE_ZFNAV

#include "sens.h"
#include "sens_io.h"

#include "hardware/hardwareif.h"

#include "algo/tracking.h"
#include "algo/AHRS.h"
#include "algo/barometer.h"
#include "algo/gpstools.h"
#include "algo/magCalibration.h"

#endif // ENABLE_ZFNAV
