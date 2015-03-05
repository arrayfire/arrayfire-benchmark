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

// Macro to simplify the creation of 1D resize tests
#define RESIZE_BENCHMARK_1D(benchmarkName, resizeMethod, dataType, scale)  \
BENCHMARK_F( Resize , Resize_1D_##dataType##_##benchmarkName##_##resizeMethod ,  \
    Fixture_1D_##dataType, samples, operations)             \
{ \
    dim4 A_size  = this->A.dims();                                  \
    array B = resize(A, A_size.dims[0] * scale, resizeMethod ); \
    B.eval();                                                       \
    af::sync();                                                     \
} \

// Resize baseline, it does nothing non-standard
BASELINE_F(Resize, Baseline_1D, Fixture_1D_f32, samples, operations) {}

// 32-bit benchmarks
RESIZE_BENCHMARK_1D(Shrink, AF_INTERP_NEAREST, f32, 0.5)
RESIZE_BENCHMARK_1D(Expand, AF_INTERP_NEAREST, f32, 2.0)
RESIZE_BENCHMARK_1D(Shrink, AF_INTERP_LINEAR,  f32, 0.5)
RESIZE_BENCHMARK_1D(Expand, AF_INTERP_LINEAR,  f32, 2.0)
// 64-bit benchmarks
RESIZE_BENCHMARK_1D(Shrink, AF_INTERP_NEAREST, f64, 0.5)
RESIZE_BENCHMARK_1D(Expand, AF_INTERP_NEAREST, f64, 2.0)
RESIZE_BENCHMARK_1D(Shrink, AF_INTERP_LINEAR,  f64, 0.5)
RESIZE_BENCHMARK_1D(Expand, AF_INTERP_LINEAR,  f64, 2.0)

// Macro to simplify the creation of 2D resize tests
#define RESIZE_BENCHMARK_2D(benchmarkName, resizeMethod, dataType, scale)  \
BASELINE_F( Resize , benchmarkName##_2D_##dataType##_##resizeMethod ,  \
    Fixture_2D_##dataType, samples, operations)             \
{ \
    dim4 A_size  = this->A.dims();                                  \
    array B = resize(A, A_size.dims[0] * scale, A_size.dims[1] * scale, resizeMethod ); \
    B.eval();                                                       \
    af::sync();                                                     \
} \

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
