/**
\brief  locale definitipnen
        nicht nach aussen sichtbar
*/

#ifndef CONFIG_ZFS_H
#define CONFIG_ZFS_H

#include "config.h" // global

#ifdef ZFS_5003_SUPPORT
# ifndef HATCHECK
#  define HATCHECK
# endif
#endif

#ifdef ZFS_COLOR_SUPPORT
# define ZFS_COLOR     // farbige scans unterstuetzen
#endif


#ifndef _WINDOWS
#ifndef MAX_PATH
#define MAX_PATH 256
#endif
#endif

#endif // CONFIG_ZFS_H
