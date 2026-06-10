#pragma once

#if defined(_WIN32) // windows 32bit or 64bit
#include "compiler_windows.h"
#else
#include "compiler_linux.h"
#endif

#ifdef _WIN32
#define ZF_DEPRECATED(msg) __declspec(deprecated(msg))
#else
#define ZF_DEPRECATED(msg) __attribute__((deprecated(msg)))
#endif

