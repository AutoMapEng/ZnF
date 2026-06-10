# SET( USE_OPENCV TRUE ) # required for reading tiff images for infrared example. Or use different reader. Remove it when you dont want read infrared example
SET( USE_ZFSFILTER TRUE ) # required for example_profiler_filter (profiler example) and example_imager_filter (imager example).


IF( NO_SAMPLE )
    include_directories(../..)
	project( ${ZFSSAMPLE_NAME} )
ELSE()
    cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

    set(ZFSSAMPLE_NAME SampleZFS)
    set(ZFS_NAME zfs)
    set(ZFCORE_NAME zfcore)
    set(FILTERDLL_NAME zfsfilter)
    set(ZFTOOLS_NAME zftools)
	set(ZFCOMMON_NAME zfcommon)

	project( ${ZFSSAMPLE_NAME} )

    if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set( MY_OUTPUT_ADD vs2008_win64 CACHE STRING "lib directory debends on Visual Studio version" )
        if(${CMAKE_GENERATOR} MATCHES ".*(Win64).*")
            set(  MY_GLOBAL_DEF  -DWIN64 -D_WINDOWS -D_CRT_SECURE_NO_WARNINGS)
            set( MY_USE_WIN64 1)
        else()
            set( MY_GLOBAL_DEF -D_WINDOWS -D_CRT_SECURE_NO_WARNINGS)
        endif()

        if( MY_USE_WIN64 )
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t")
        else( MY_USE_WIN64 )
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t /arch:SSE2")
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ")
        endif( MY_USE_WIN64 )
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Oi /Ot /Ox /GS-")

        add_definitions(-D_CRT_SECURE_NO_WARNINGS)

        set(MY_EXE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../Exe/${MY_OUTPUT_ADD})
        set(MY_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../Lib/${MY_OUTPUT_ADD})

        include_directories(../..)
    else()
        find_package(zf-zfs-sdk)
    endif()

    if(COMMAND cmake_policy)
        cmake_policy(SET CMP0003 NEW)
    endif()

    if(COMMAND cmake_policy)
      cmake_policy(SET CMP0002 NEW)
    endif()

ENDIF()

if(MY_TEST)
    message( STATUS "Start create ${ZFSSAMPLE_NAME}" )
    message(STATUS " ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}")
endif()


if (WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${MY_EXE_PATH}/debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${MY_EXE_PATH}/release)
else ()
    # A lot of variables are defined and never used in this example
    # Normally this is a thing to avoid, bute here...
    add_compile_options(-Wno-unused-variable)
    add_compile_options(-Wno-unused-value)
    add_compile_options(-Wno-unused-but-set-variable)
endif ()

set(ZFSAMPLE_SOURCES_CPP
  main.cpp
  example_profiler.cpp
  example_imager.cpp
  example_infrared.cpp
  example_tools.cpp
  example_layer.cpp
  example_write.cpp
  example_lineheader.cpp
)
set(ZFSAMPLE_SOURCES_H
  example_profiler.h
  example_imager.h
  example_infrared.h
  example_tools.h
  example_layer.h
  example_write.h
  example_lineheader.h
)
add_definitions(-D_CONSOLE ${MY_GLOBAL_DEF})


link_directories(${MY_STD_LINK_DIR})

set(CMAKE_BINARY_DIR "${CMAKE_BINARY_DIR}${MY_OUTPUT_ADD}")
add_executable(${PROJECT_NAME} ${ZFSAMPLE_SOURCES_CPP} ${ZFSAMPLE_SOURCES_H})


if (NO_SAMPLE OR (${CMAKE_SYSTEM_NAME} MATCHES "Linux"))
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            ${ZFCORE_NAME}
            ${ZFS_NAME}
            ${FILTERDLL_NAME}
            ${ZFTOOLS_NAME}
            $<$<BOOL:${UNIX}>:pthread>
            ${ZFCOMMON_NAME})
else()

    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            optimized ${MY_LIB_PATH}/release/${ZFCORE_NAME}.lib
            debug ${MY_LIB_PATH}/debug/${ZFCORE_NAME}.lib
            optimized ${MY_LIB_PATH}/release/${ZFS_NAME}.lib
            debug ${MY_LIB_PATH}/debug/${ZFS_NAME}.lib
            optimized ${MY_LIB_PATH}/release/${ZFTOOLS_NAME}.lib
            debug ${MY_LIB_PATH}/debug/${ZFTOOLS_NAME}.lib
		    optimized ${MY_LIB_PATH}/release/${ZFCOMMON_NAME}.lib
		    debug ${MY_LIB_PATH}/debug/${ZFCOMMON_NAME}.lib
    )
    message(STATUS "libs: ${ZFS_NAME} ${FILTERDLL_NAME}")

endif()


if(USE_OPENCV)
    add_definitions(-DUSE_OPENCV)
    find_package(OpenCV)
    include_directories(${OPENCV_DIR})
    target_link_libraries(${PROJECT_NAME} private ${OPENCV_LINK_LIBRARIES})
endif(USE_OPENCV)

if(USE_ZFSFILTER)
    add_definitions(-DUSE_ZFSFILTER)
    # additional library required

    if (NO_SAMPLE OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_link_libraries(${PROJECT_NAME} PRIVATE optimized ${FILTERDLL_NAME})
    else()
        target_link_libraries( ${PROJECT_NAME} PUBLIC
            optimized ${MY_LIB_PATH}/release/${FILTERDLL_NAME}.lib
            debug ${MY_LIB_PATH}/debug/${FILTERDLL_NAME}.lib
        )
    endif()
endif(USE_ZFSFILTER)


install(DIRECTORY .
    DESTINATION ${CMAKE_INSTALL_DATADIR}/zf-zfs-sdk/example/zfs)
