cmake_minimum_required(VERSION 3.0)

cmake_policy(SET CMP0020 NEW)
if(POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

if(NOT DEFINED SOCKETSAMPLE_QT_NAME)
  set(SOCKETSAMPLE_QT_NAME SampleCtrlSocketQT)
endif()

if(WIN32)
    if(NOT DEFINED NO_SAMPLE)
        set(USE_QT5 1)
        set(MY_EXE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../Exe/${MY_OUTPUT_ADD})
        set(MY_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/${MY_OUTPUT_ADD})
        set(ZFCORE_NAME
            optimized ${MY_LIB_PATH}/release/zfcore.lib
            debug ${MY_LIB_PATH}/debug/zfcore.lib
        )
        set(ZFS_NAME
            optimized ${MY_LIB_PATH}/release/zfs.lib
            debug ${MY_LIB_PATH}/debug/zfs.lib
        )
        set(ZFSTOOLS_NAME
            optimized ${MY_LIB_PATH}/release/zfstools.lib
            debug ${MY_LIB_PATH}/debug/zfstools.lib
        )
        set(LRCSOCKET_NAME
            optimized ${MY_LIB_PATH}/release/lrcsocket.lib
            debug ${MY_LIB_PATH}/debug/lrcsocket.lib
        )
        include_directories("../../")
    endif()
else()
    if(NOT DEFINED NO_SAMPLE)
        find_package(zf-zfs-sdk)
    endif()
    set(ZFS_NAME zfs)
    set(ZFCORE_NAME zfcore)
    set(ZFSTOOLS_NAME zfstools)
    set(LRCSOCKET_NAME lrcsocket)
    #set(QTOOLS_NAME qtools)
endif()

project( ${SOCKETSAMPLE_QT_NAME} )

set(QT_USE_QTNETWORK TRUE)

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${MY_LIB_PATH}/debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${MY_EXE_PATH}/debug)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${MY_LIB_PATH}/release)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${MY_EXE_PATH}/release)
endif()

set(CTRLSAMPLE_SOURCES_CPP
    main.cpp
    dialog/mainwindow.cpp
    if.cpp)

set(CTRLSAMPLE_SOURCES_H if.h)
set(CTRLSAMPLE_SOURCE_MOC_H dialog/mainwindow.h)
set(CTRLSAMPLE_SOURCE_UI dialog/mainwindow.ui)

find_package(Qt5 COMPONENTS Widgets Svg)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
set(QT_LIBRARIES Qt5::Widgets Qt5::Svg)

QT5_WRAP_CPP( CTRLSAMPLE_MOC_SRCS ${CTRLSAMPLE_SOURCE_MOC_H} )
QT5_WRAP_UI( CTRLSAMPLE_UI_SRCS  ${CTRLSAMPLE_SOURCE_UI} )

set(CTRLSAMPLE_SOURCES_RES Icon.rc)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

if(NOT USE_QT5)
    add_definitions(
        -DQT_ACCESSIBILITY_SUPPORT
        -DQT_NO_DEBUG
        -DQT_THREAD_SUPPORT)
endif()

add_definitions(
  ${MY_GLOBAL_DEF}
)

IF( NO_SAMPLE )
add_definitions(
	-DUSE_ZF_FILTER_DIALOG
	)
ENDIF()

link_directories(
  ${MY_STD_LINK_DIR}
)

source_group(Resources FILES ${CTRLSAMPLE_SOURCES_RES} ${CTRLSAMPLER_SOURCES_RES_QT})
source_group(Generated FILES ${CTRLSAMPLE_MOC_SRCS} ${CTRLSAMPLE_RES_FILE})
source_group(UI FILES ${CTRLSAMPLE_SOURCE_UI})

add_executable(${PROJECT_NAME} WIN32
    ${CTRLSAMPLE_SOURCE_MOC_H}
    ${CTRLSAMPLE_SOURCES_CPP}
    ${CTRLSAMPLE_SOURCES_H}
    ${CTRLSAMPLE_MOC_SRCS}
    ${CTRLSAMPLE_SOURCES_RES}
    ${CTRLSAMPLE_UI_SRCS}
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    #${QTOOLS_NAME}
    ${ZFCORE_NAME}
    ${ZFS_NAME}
    ${LRCSOCKET_NAME}
    ${QT_LIBRARIES}
)
IF( NO_SAMPLE )
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${ZFSTOOLS_NAME} )
ENDIF()

set_target_properties(${PROJECT_NAME} PROPERTIES
    DEBUG_OUTPUT_NAME ${PROJECT_NAME}
    RELEASE_OUTPUT_NAME ${PROJECT_NAME})


if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

install(DIRECTORY .
    DESTINATION ${CMAKE_INSTALL_DATADIR}/zf-zfs-sdk/example/ctrlsocketqt)

