#pragma once

#if !defined(WIDEN)
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#endif

#define ext_nav_sensors ".dat"
#define wext_nav_sensors WIDEN(ext_nav_sensors)

