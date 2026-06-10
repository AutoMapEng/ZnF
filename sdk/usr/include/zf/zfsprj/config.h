#pragma once

#ifdef _WINDOWS
#ifndef CHECKLINUX              //Flag um die Abhängikeiten unter Linux to testen
#define ZFSPRJ_BACKUP			// backup project bevore storing
#define ZFSPRJ_DXF_SUPPORT
#define ZFSPRJ_VRML_SUPPORT
//#define ZFSPRJ_ZFC_SUPPORT
#define ZFSPRJ_E57_SUPPORT
#define ZFSPRJ_PTX_SUPPORT
#define SUPPORT_PRJ_UNDO
#define SUPPORT_GROUPS
#define ZFSPRJ_SUPPORT_ZFI
#define ZFPRJ_SLAM_SUPPORT
#define ZFPRJ_FILEEXPORT
#define ZFPRJ_WINDOWS
#ifdef EN_ZFDB
#define ZFPRJ_ZFDB
#endif
#endif
#endif

#ifdef LINUX
#endif

