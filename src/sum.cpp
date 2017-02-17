/*
 * matrix_multiply.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: bkloppenborg
 */

#include <cstdint>
#include <celero/Celero.h>
#include <arrayfire.h>
#include "fixtures.h" // defines most test figures

using namespace af;

extern unsigned int samples;
extern unsigned int iterations;

BASELINE_F(Sum, Baseline, AF_Fixture, samples, iterations) { }

#define Sum_BENCHMARK(ctype, dataType)                                                              \
BENCHMARK_F( Sum, Sum_1D_##dataType, Fixture_1D_##dataType, samples, iterations)                  \
{                                                                                                   \
    array B = sum(A);                                                                               \
    B.eval();                                                                                       \
}                                                                                                   \
BENCHMARK_F( Sum, Sum_2D_##dataType, Fixture_2D_##dataType, samples, iterations)                  \
{                                                                                                   \
    array B = sum(A);                                                                               \
    B.eval();                                                                                       \
}                                                                                                   \

Sum_BENCHMARK(uint8_t, u8)
Sum_BENCHMARK(int16_t, s16)
Sum_BENCHMARK(int32_t, s32)
Sum_BENCHMARK(long long, s64)
Sum_BENCHMARK(float, f32)
Sum_BENCHMARK(double, f64)
