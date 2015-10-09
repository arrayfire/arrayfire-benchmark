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
extern unsigned int operations;

// Resize baseline, it does nothing non-standard
BASELINE_F(Resize, Baseline, Fixture_1D_f32, samples, operations) {}

// Macro to simplify the creation of 2D resize tests
#define RESIZE_BENCHMARK_2D(benchmarkName, resizeMethod, dataType, scale)   \
BENCHMARK_F( Resize , benchmarkName##_2D_##dataType##_##resizeMethod ,      \
    Fixture_2D_##dataType, samples, operations)                             \
{                                                                           \
    array B = af::resize(scale, A, resizeMethod );                          \
    B.eval();                                                               \
}

// 32-bit benchmarks
RESIZE_BENCHMARK_2D(Shrink, AF_INTERP_NEAREST,   f32, 0.5)
RESIZE_BENCHMARK_2D(Expand, AF_INTERP_NEAREST,   f32, 2.0)
RESIZE_BENCHMARK_2D(Shrink, AF_INTERP_BILINEAR,  f32, 0.5)
RESIZE_BENCHMARK_2D(Expand, AF_INTERP_BILINEAR,  f32, 2.0)
// 64-bit benchmarks
RESIZE_BENCHMARK_2D(Shrink, AF_INTERP_NEAREST,   f64, 0.5)
RESIZE_BENCHMARK_2D(Expand, AF_INTERP_NEAREST,   f64, 2.0)
RESIZE_BENCHMARK_2D(Shrink, AF_INTERP_BILINEAR,  f64, 0.5)
RESIZE_BENCHMARK_2D(Expand, AF_INTERP_BILINEAR,  f64, 2.0)
