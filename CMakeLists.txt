CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(af_benchmark)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

## Include and build the Celero benchmark framework
if(NOT TARGET celero)
    ADD_SUBDIRECTORY (lib/celero)
endif()
include_directories(lib/celero/include)

add_subdirectory(src)
