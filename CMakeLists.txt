CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(af_benchmark)

# Compile with C++11 support.
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

if(WIN32)
    ADD_DEFINITIONS(-DOS_WIN -DNOMINMAX)
endif()

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(BENCHMARK_DATA_DIRECTORY "${CMAKE_SOURCE_DIR}/data/single_frames/")
ADD_DEFINITIONS(-DBENCHMARK_DATA_DIRECTORY=\"${BENCHMARK_DATA_DIRECTORY}\" -DCELERO_STATIC)
LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

## Include and build the Celero benchmark framework
if(NOT TARGET celero)
    # Disable some default options from Celero
    SET(CELERO_ENABLE_EXPERIMENTS OFF CACHE BOOL "Automatically build Celero examples")
    SET(CELERO_COMPILE_DYNAMIC_LIBRARIES OFF CACHE BOOL "Build Static Celero Library")
    ADD_SUBDIRECTORY (lib/celero)
endif()
include_directories(lib/celero/include)

add_subdirectory(src)
