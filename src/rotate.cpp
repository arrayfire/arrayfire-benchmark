/*
 * matrix_multiply.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: bkloppenborg
 */

#include <math.h>
#include <cstdint>
#include <celero/Celero.h>
#include <arrayfire.h>
#include "fixtures.h" // defines most test figures

using namespace af;

extern unsigned int samples;
extern unsigned int iterations;

BASELINE_F(Rotate, Baseline, AF_Fixture_2D, samples, iterations) { }

#define Rotate_BENCHMARK(ctype, dataType)                                                           \
BENCHMARK_F( Rotate, Rotate_##dataType##_INTERP_NEAREST, Fixture_2D_##dataType, samples, iterations)\
{                                                                                                   \
    array B = rotate(A, af::Pi / 4.0f);                                                             \
    B.eval();                                                                                       \
}                                                                                                   \

Rotate_BENCHMARK(std::uint8_t, u8)
Rotate_BENCHMARK(std::int16_t, s16)
Rotate_BENCHMARK(std::int32_t, s32)
Rotate_BENCHMARK(std::int64_t, s64)
Rotate_BENCHMARK(float, f32)
Rotate_BENCHMARK(double, f64)
