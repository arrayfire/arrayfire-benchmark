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

BASELINE_F(Transpose, Baseline, AF_Fixture_2D, samples, iterations) { }

#define Transpose_BENCHMARK(ctype, dataType)                                                              \
BENCHMARK_F( Transpose, Transpose_##dataType, Fixture_2D_##dataType, samples, iterations)                 \
{                                                                                                         \
    array B = transpose(A);                                                                               \
    B.eval();                                                                                             \
}                                                                                                         \

Transpose_BENCHMARK(uint8_t, u8)
Transpose_BENCHMARK(int16_t, s16)
Transpose_BENCHMARK(int32_t, s32)
Transpose_BENCHMARK(int64_t, s64)
Transpose_BENCHMARK(float, f32)
Transpose_BENCHMARK(double, f64)
