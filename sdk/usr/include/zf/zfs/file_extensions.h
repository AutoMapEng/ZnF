#pragma once

#if !defined(WIDEN)
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#endif


// zf scan file
#if !defined(ext_zfs)
#define ext_zfs ".zfs"
#endif
#define wext_zfs WIDEN(ext_zfs)

// image/file container
#if !defined(ext_zfi)
#define ext_zfi ".zfi"
#endif
#define wext_zfi WIDEN(ext_zfi)

// layer of zfs
#if !defined(ext_zfs_layer)
#define ext_zfs_layer ".zfl"
#endif
#define wext_zfs_layer WIDEN(ext_zfs_layer)

// time api
#if !defined(ext_zfs_time)
#define ext_zfs_time ".time"
#endif
#define wext_zfs_time WIDEN(ext_zfs_time)

// line position temp file
#if !defined(ext_zfs_pos)
#define ext_zfs_pos  ".pos"
#endif
#define wext_zfs_pos  WIDEN(ext_zfs_pos)

// temp folder for scans
#if !defined(folder_temp)
#define folder_temp  ".temp"
#endif
#define wfolder_temp  WIDEN(folder_temp)


// filter

// filter folder
#if !defined(folder_zfs_filter)
#define folder_zfs_filter  ".filter"
#endif
#define wfolder_zfs_filter  WIDEN(folder_zfs_filter)

// filter params file
#if !defined(ext_zfs_filter_param)
#define ext_zfs_filter_params  ".filter-params.xml"
#endif
#define wext_zfs_filter_params  WIDEN(ext_zfs_filter_params)

// filter dirty flag
#if !defined(ext_zfs_filter_dirty)
#define ext_zfs_filter_dirty  ".dirty"
#endif
#define wext_zfs_filter_dirty  WIDEN(ext_zfs_filter_dirty)

// filter result file
#if !defined(ext_zfs_filter_result)
#define ext_zfs_filter_result  ".filter-result.txt"
#endif
#define wext_zfs_filter_result  WIDEN(ext_zfs_filter_result)

#if !defined(ext_topcut)
#define ext_topcut  ".topcut"
#define wext_topcut  WIDEN(ext_topcut)
#endif

