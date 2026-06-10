cmake_minimum_required(VERSION 3.10)
project(zf_acquire CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# build with g++:  cmake -S cpp -B cpp/build -DCMAKE_CXX_COMPILER=g++
#                  cmake --build cpp/build
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)
find_package(X11)

add_executable(acquire acquire.cpp ZnF9020.cpp)
target_compile_options(acquire PRIVATE -Wall -Wextra -O2)
target_link_libraries(acquire PRIVATE ZLIB::ZLIB Threads::Threads)

if(X11_FOUND)
    add_executable(plotter plotter.cpp ZnF9020.cpp)
    target_compile_options(plotter PRIVATE -Wall -Wextra -O2)
    target_link_libraries(plotter PRIVATE ZLIB::ZLIB X11::X11 Threads::Threads)
else()
    message(WARNING "libX11 dev headers not found - 'plotter' target skipped")
endif()
