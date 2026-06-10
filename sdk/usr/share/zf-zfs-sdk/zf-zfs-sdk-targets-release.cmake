#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "zfcommon" for configuration "Release"
set_property(TARGET zfcommon APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(zfcommon PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libzfcommon.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS zfcommon )
list(APPEND _IMPORT_CHECK_FILES_FOR_zfcommon "${_IMPORT_PREFIX}/lib/libzfcommon.a" )

# Import target "zfcore" for configuration "Release"
set_property(TARGET zfcore APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(zfcore PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libzfcore.so.11.0.2503"
  IMPORTED_SONAME_RELEASE "libzfcore.so.11.0.2503"
  )

list(APPEND _IMPORT_CHECK_TARGETS zfcore )
list(APPEND _IMPORT_CHECK_FILES_FOR_zfcore "${_IMPORT_PREFIX}/lib/libzfcore.so.11.0.2503" )

# Import target "zfssim" for configuration "Release"
set_property(TARGET zfssim APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(zfssim PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libzfssim.so.11.0.2503"
  IMPORTED_SONAME_RELEASE "libzfssim.so.11.0.2503"
  )

list(APPEND _IMPORT_CHECK_TARGETS zfssim )
list(APPEND _IMPORT_CHECK_FILES_FOR_zfssim "${_IMPORT_PREFIX}/lib/libzfssim.so.11.0.2503" )

# Import target "zfs" for configuration "Release"
set_property(TARGET zfs APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(zfs PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libzfs.so.11.0.2503"
  IMPORTED_SONAME_RELEASE "libzfs.so.11.0.2503"
  )

list(APPEND _IMPORT_CHECK_TARGETS zfs )
list(APPEND _IMPORT_CHECK_FILES_FOR_zfs "${_IMPORT_PREFIX}/lib/libzfs.so.11.0.2503" )

# Import target "zfsfilter" for configuration "Release"
set_property(TARGET zfsfilter APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(zfsfilter PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libzfsfilter.so.11.0.2503"
  IMPORTED_SONAME_RELEASE "libzfsfilter.so.11.0.2503"
  )

list(APPEND _IMPORT_CHECK_TARGETS zfsfilter )
list(APPEND _IMPORT_CHECK_FILES_FOR_zfsfilter "${_IMPORT_PREFIX}/lib/libzfsfilter.so.11.0.2503" )

# Import target "zfsprj" for configuration "Release"
set_property(TARGET zfsprj APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(zfsprj PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "Qt5::Widgets;Qt5::Svg"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libzfsprj.so.11.0.2503"
  IMPORTED_SONAME_RELEASE "libzfsprj.so.11.0.2503"
  )

list(APPEND _IMPORT_CHECK_TARGETS zfsprj )
list(APPEND _IMPORT_CHECK_FILES_FOR_zfsprj "${_IMPORT_PREFIX}/lib/libzfsprj.so.11.0.2503" )

# Import target "zfxyz" for configuration "Release"
set_property(TARGET zfxyz APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(zfxyz PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libzfxyz.so.11.0.2503"
  IMPORTED_SONAME_RELEASE "libzfxyz.so.11.0.2503"
  )

list(APPEND _IMPORT_CHECK_TARGETS zfxyz )
list(APPEND _IMPORT_CHECK_FILES_FOR_zfxyz "${_IMPORT_PREFIX}/lib/libzfxyz.so.11.0.2503" )

# Import target "zfnav" for configuration "Release"
set_property(TARGET zfnav APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(zfnav PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "Qt5::Network;Qt5::Core;zftools;opencv_core;opencv_imgproc"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libzfnav.so.11.0.2503"
  IMPORTED_SONAME_RELEASE "libzfnav.so.11.0.2503"
  )

list(APPEND _IMPORT_CHECK_TARGETS zfnav )
list(APPEND _IMPORT_CHECK_FILES_FOR_zfnav "${_IMPORT_PREFIX}/lib/libzfnav.so.11.0.2503" )

# Import target "lrcsocket" for configuration "Release"
set_property(TARGET lrcsocket APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(lrcsocket PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/liblrcsocket.so.11.0.2503"
  IMPORTED_SONAME_RELEASE "liblrcsocket.so.11.0.2503"
  )

list(APPEND _IMPORT_CHECK_TARGETS lrcsocket )
list(APPEND _IMPORT_CHECK_FILES_FOR_lrcsocket "${_IMPORT_PREFIX}/lib/liblrcsocket.so.11.0.2503" )

# Import target "zftools" for configuration "Release"
set_property(TARGET zftools APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(zftools PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "zfs;zfcore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libzftools.so.11.0.2503"
  IMPORTED_SONAME_RELEASE "libzftools.so.11.0.2503"
  )

list(APPEND _IMPORT_CHECK_TARGETS zftools )
list(APPEND _IMPORT_CHECK_FILES_FOR_zftools "${_IMPORT_PREFIX}/lib/libzftools.so.11.0.2503" )

# Import target "qtools" for configuration "Release"
set_property(TARGET qtools APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(qtools PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "zfsprj"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libqtools.so.11.0.2503"
  IMPORTED_SONAME_RELEASE "libqtools.so.11.0.2503"
  )

list(APPEND _IMPORT_CHECK_TARGETS qtools )
list(APPEND _IMPORT_CHECK_FILES_FOR_qtools "${_IMPORT_PREFIX}/lib/libqtools.so.11.0.2503" )

# Import target "zfstools" for configuration "Release"
set_property(TARGET zfstools APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(zfstools PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "zfsprj;Qt5::Widgets;zfsfilter"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libzfstools.so.11.0.2503"
  IMPORTED_SONAME_RELEASE "libzfstools.so.11.0.2503"
  )

list(APPEND _IMPORT_CHECK_TARGETS zfstools )
list(APPEND _IMPORT_CHECK_FILES_FOR_zfstools "${_IMPORT_PREFIX}/lib/libzfstools.so.11.0.2503" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
