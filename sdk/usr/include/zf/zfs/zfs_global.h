#ifndef ZFS_GLOBAL_H
#define ZFS_GLOBAL_H

//#include <QtCore/qglobal.h>

#if defined(ZFS_LIBRARY)
#  define ZFSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ZFSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ZFS_GLOBAL_H
