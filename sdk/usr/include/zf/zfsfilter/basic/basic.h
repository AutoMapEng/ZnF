#pragma once

#include <zfs/mask.h>
#include <zfsfilter/filterapi.h>

#define ENABLE_SEL_3DRECT

#define ENABLE_NONJMP           // noniusspruenge zurueckrechnen (frueher:noniusjumps stored in counterbyte maskieren)
#define ENABLE_FILTER_INVALID
#define ENABLE_FILTER_NOISE
#define ENABLE_FILTER_EINDEUTIGKEIT_RF
#define ENABLE_MODIFYRANGE      // eindeutikeitsinterval und nonius jumps plus/minus

#define FILTER_DYN_COMPENSATOR_ERROR    // lines which dynamic tilt changes are too high, can be filtered


