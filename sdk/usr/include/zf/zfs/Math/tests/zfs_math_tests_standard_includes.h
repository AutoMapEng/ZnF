#pragma once

#include <zfs/Math/math/def.h>

#if defined(_CPP11) && defined(_TIME_MEASUREMENT)
   #include <chrono>
   #include <iostream>
#else
   #undef _TIME_MEASUREMENT
#endif

#if defined(_CPP11) && defined(_THREADING)
   #include <thread>
#else
   #undef _THREADING
#endif

#if defined(_CPP11) && defined(_RANDOM)
   #include <random>
#else
   #undef _RANDOM
#endif