CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(af_benchmark)

## Include and build the Celero benchmark framework
if(NOT TARGET celero)
    ADD_SUBDIRECTORY (lib/celero)
endif()
include_directories(lib/celero/include)

add_subdirectory(src)
