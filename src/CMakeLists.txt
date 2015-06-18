CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(af_benchmark)

FIND_PACKAGE(ArrayFire REQUIRED)
INCLUDE_DIRECTORIES(${ArrayFire_INCLUDE_DIRS})

FIND_PACKAGE( Boost COMPONENTS program_options system filesystem regex REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

find_package(Threads REQUIRED)

FILE(GLOB SOURCE "*.cpp")
ADD_LIBRARY(BENCHMARKS OBJECT ${SOURCE})

# Build benchmarks linking specifically with designated backends
if(${ArrayFire_CPU_FOUND})
    MESSAGE(STATUS "ArrayFire CPU backend found. Enabling CPU benchmark")
    ADD_EXECUTABLE(benchmark_cpu $<TARGET_OBJECTS:BENCHMARKS>)
    TARGET_LINK_LIBRARIES(benchmark_cpu celero ${ArrayFire_CPU_LIBRARIES} 
        ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
ENDIF()

if(${ArrayFire_OpenCL_FOUND})
    MESSAGE(STATUS "ArrayFire OpenCL backend found. Enabling OpenCL benchmark")
    ADD_EXECUTABLE(benchmark_opencl $<TARGET_OBJECTS:BENCHMARKS>)
    TARGET_LINK_LIBRARIES(benchmark_opencl celero ${ArrayFire_OpenCL_LIBRARIES} 
        ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
ENDIF()

if(${ArrayFire_CUDA_FOUND})
    FIND_PACKAGE(CUDA REQUIRED)
    FIND_PACKAGE(NVVM REQUIRED)
    MESSAGE(STATUS ${CUDA_TOOLKIT_ROOT_DIR})
    MESSAGE(STATUS "ArrayFire CUDA found. Enabling CUDA benchmark")
    ADD_EXECUTABLE(benchmark_cuda $<TARGET_OBJECTS:BENCHMARKS>)
    TARGET_LINK_LIBRARIES(benchmark_cuda celero ${ArrayFire_CUDA_LIBRARIES} 
        ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CUDA_LIBRARIES} ${NVVM_LIB})
ENDIF()
