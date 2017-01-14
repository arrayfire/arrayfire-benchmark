/*
 * matrix_multiply.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: bkloppenborg
 */

#include <celero/Celero.h>
#include <arrayfire.h>
#include "fixtures.h" // defines most test figures
using namespace af;

extern unsigned int samples;
extern unsigned int iterations;

// Resize baseline, it does nothing non-standard
BASELINE_F(Resize, Baseline, AF_Fixture_1D, samples, iterations) {}

// Macro to simplify the creation of 2D resize tests
#define RESIZE_BENCHMARK_2D(benchmarkName, resizeMethod, dataType, scale)   \
BENCHMARK_F( Resize , benchmarkName##_2D_##dataType##_##resizeMethod ,      \
    Fixture_2D_##dataType, samples, iterations)                             \
{                                                                           \
    array B = af::resize(scale, A, resizeMethod );                          \
    B.eval();                                                               \
}

#define RESIZE_BENCHMARK_2D_SET(dataType)                        \
RESIZE_BENCHMARK_2D(Shrink, AF_INTERP_NEAREST,   dataType, 0.5)  \
RESIZE_BENCHMARK_2D(Expand, AF_INTERP_NEAREST,   dataType, 2.0)  \
RESIZE_BENCHMARK_2D(Shrink, AF_INTERP_BILINEAR,  dataType, 0.5)  \
RESIZE_BENCHMARK_2D(Expand, AF_INTERP_BILINEAR,  dataType, 2.0)  \

RESIZE_BENCHMARK_2D_SET(u8)
RESIZE_BENCHMARK_2D_SET(s16)
RESIZE_BENCHMARK_2D_SET(s32)
RESIZE_BENCHMARK_2D_SET(s64)
RESIZE_BENCHMARK_2D_SET(f32)
RESIZE_BENCHMARK_2D_SET(f64)
