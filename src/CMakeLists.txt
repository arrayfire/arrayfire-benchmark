CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(af_benchmark)

FIND_PACKAGE(ArrayFire REQUIRED)

FILE(GLOB SOURCE "*.cpp")

# Build benchmarks linking specifically with designated backends
if(ArrayFire_CPU_FOUND)
    MESSAGE(STATUS "ArrayFire CPU backend found. Enabling CPU benchmark")
    ADD_EXECUTABLE(benchmark_cpu ${SOURCE})
    MESSAGE(STATUS ${ArrayFire_CPU_LIBRARIES})
    TARGET_LINK_LIBRARIES(benchmark_cpu celero ${ArrayFire_CPU_LIBRARIES})
ENDIF()

if(ArrayFire_OPENCL_FOUND)
    MESSAGE(STATUS "ArrayFire OpenCL backend found. Enabling OpenCL benchmark")
    ADD_EXECUTABLE(benchmark_opencl ${SOURCE})
    TARGET_LINK_LIBRARIES(benchmark_opencl celero ${ArrayFire_OPENCL_LIBRARIES})
ENDIF()

if(ArrayFire_CUDA_FOUND)
    MESSAGE(STATUS "ArrayFire CUDA found. Enabling CUDA benchmark")
    ADD_EXECUTABLE(benchmark_cuda ${SOURCE})
    TARGET_LINK_LIBRARIES(benchmark_cuda celero ${ArrayFire_CUDA_LIBRARIES})
ENDIF()

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
)
