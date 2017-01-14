/*
 * matrix_multiply.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: bkloppenborg
 */

#include <math.h>
#include <celero/Celero.h>
#include <arrayfire.h>
#include "fixtures.h" // defines most test figures

using namespace af;

extern unsigned int samples;
extern unsigned int iterations;

// Benchmarks for 32-bit floating point tests
BASELINE_F(Sort, Baseline, Fixture_1D_f32, samples, iterations) { }

#define SORT_BENCHMARK(dataType)                                                           \
BENCHMARK_F(Sort, Sort_##dataType##_ACENDING , Fixture_1D_##dataType, samples, iterations) \
{                                                                                          \
    array B = sort(A);                                                                     \
    B.eval();                                                                              \
}                                                                                          \
BENCHMARK_F(Sort, Sort_##dataType##_DECENDING , Fixture_1D_##dataType, samples, iterations)\
{                                                                                          \
    array B = sort(A, false);                                                              \
    B.eval();                                                                              \
}                                                                                          \

SORT_BENCHMARK(u8)
SORT_BENCHMARK(u16)
SORT_BENCHMARK(s16)
SORT_BENCHMARK(u32)
SORT_BENCHMARK(s32)
SORT_BENCHMARK(u64)
SORT_BENCHMARK(s64)
SORT_BENCHMARK(f32)
SORT_BENCHMARK(f64)
