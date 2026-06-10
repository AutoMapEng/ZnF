if( NO_SAMPLE )

    # Z+F local compile stuff
    include_directories(../..)

else()
    cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

    if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set( MY_OUTPUT_ADD vs2008_win64 CACHE STRING "lib directory debends on Visual Studio version" )
        if(${CMAKE_GENERATOR} MATCHES ".*(Win64).*")
            set(MY_GLOBAL_DEF -DWIN64 -D_WINDOWS -D_CRT_SECURE_NO_WARNINGS)
            set(MY_USE_WIN64 1)
        else()
            set(MY_GLOBAL_DEF -D_WINDOWS -D_CRT_SECURE_NO_WARNINGS)
        endif()
    endif()

    if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
    endif()

    if(COMMAND cmake_policy)
      cmake_policy(SET CMP0002 NEW)
    endif()

    set(ZFPRJ_SAMPLE_NAME SampleZFPRJ)
    set(ZFS_NAME zfs)
    set(ZFSPRJ_NAME zfsprj)
    set(ZFCOMMON_NAME zfcommon)

    # schon definiert. Siehe oben
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    # Example uses OpenCV for reading tiff image
    # Please modify the settings for your environment or use a different library

    if (WIN32)
        if(MY_USE_WIN64)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t /arch:SSE2")
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ")
        endif()

        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Oi /Ot /Ox /GS-")
        set(MY_EXE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../Exe/${MY_OUTPUT_ADD})
        set(MY_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/${MY_OUTPUT_ADD})
        include_directories(../..)
    else()
        find_package(zf-zfs-sdk)
    endif()
endif()

project(${ZFPRJ_SAMPLE_NAME})

if (WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${MY_EXE_PATH}/debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${MY_EXE_PATH}/release)
endif ()

find_package(Qt5 COMPONENTS Widgets Network Svg REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
set(QT_LIBRARIES Qt5::Widgets)

set(ZFPRJSAMPLE_SOURCES_CPP
  main.cpp
)

set(ZFPRJSAMPLE_SOURCES_H
)

add_definitions(
  -D_CONSOLE
  ${MY_GLOBAL_DEF}
)

include_directories(
  ${Boost_INCLUDE_DIR}
)

link_directories(
  ${MY_STD_LINK_DIR}
  ${Boost_LIBRARY_DIRS}
)

set(CMAKE_BINARY_DIR "${CMAKE_BINARY_DIR}${MY_OUTPUT_ADD}")
add_executable( ${ZFPRJ_SAMPLE_NAME} ${ZFPRJSAMPLE_SOURCES_CPP} ${ZFPRJSAMPLE_SOURCES_H})
install(DIRECTORY .
    DESTINATION ${CMAKE_INSTALL_DATADIR}/zf-zfs-sdk/example/zfprj)


if(NO_SAMPLE OR UNIX)
    target_link_libraries(${ZFPRJ_SAMPLE_NAME}
        PRIVATE
            ${ZFS_NAME}
            ${ZFSPRJ_NAME}
            ${QT_LIBRARIES}
            ${ZFCOMMON_NAME})
else()
    target_link_libraries( ${ZFPRJ_SAMPLE_NAME} optimized ${MY_LIB_PATH}/release/${ZFS_NAME}.lib
        optimized ${MY_LIB_PATH}/release/${ZFSPRJ_NAME}.lib
        debug ${MY_LIB_PATH}/debug/${ZFS_NAME}.lib
        debug ${MY_LIB_PATH}/debug/${ZFSPRJ_NAME}.lib)
endif()

set_target_properties(${ZFPRJ_SAMPLE_NAME} PROPERTIES
    DEBUG_OUTPUT_NAME ${ZFPRJ_SAMPLE_NAME}
    RELEASE_OUTPUT_NAME ${ZFPRJ_SAMPLE_NAME})
