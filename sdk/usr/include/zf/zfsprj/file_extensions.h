#pragma once

#if !defined(WIDEN)
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#endif

#if !defined(ext_zfprj)
#define ext_zfprj ".zfprj"
#endif
#define wext_zfprj WIDEN(ext_zfprj)

#if !defined(ext_zfprj_backup)
#define ext_zfprj_backup ".backup"		// zfprj backup file
#endif
#define wext_zfprj_backup WIDEN(ext_zfprj_backup)

#if !defined(folder_zfprj_backup)
#define folder_zfprj_backup	".backup"
#endif
#define wfolder_zfprj_backup WIDEN(folder_zfprj_backup)

#if !defined(ext_zfprj_lock)
#define ext_zfprj_lock ".lock"
#endif
#define wext_zfprj_lock WIDEN(ext_zfprj_lock)
