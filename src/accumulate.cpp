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

BASELINE_F(Accumulate, Baseline, AF_Fixture, samples, iterations) { }

#define Accumulate_BENCHMARK(ctype, dataType)                                                     \
BENCHMARK_F( Accumulate, Accumulate_1D_##dataType, Fixture_1D_##dataType, samples, iterations)    \
{                                                                                                 \
    array B = accum(A);                                                                           \
    B.eval();                                                                                     \
}                                                                                                 \
BENCHMARK_F( Accumulate, Accumulate_2D_##dataType, Fixture_2D_##dataType, samples, iterations)    \
{                                                                                                 \
    array B = accum(A);                                                                           \
    B.eval();                                                                                     \
}                                                                                                 \

Accumulate_BENCHMARK(uint8_t, u8)
Accumulate_BENCHMARK(int16_t, s16)
Accumulate_BENCHMARK(int32_t, s32)
Accumulate_BENCHMARK(long long, s64)
Accumulate_BENCHMARK(float, f32)
Accumulate_BENCHMARK(double, f64)
