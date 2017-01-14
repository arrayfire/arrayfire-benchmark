/*
 * bilateral.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: bkloppenborg
 */

#include <celero/Celero.h>
#include <arrayfire.h>
#include "fixtures.h"
using namespace af;

extern unsigned int samples;
extern unsigned int iterations;

BASELINE_F(BilateralFilter, Baseline, AF_Fixture_2D, samples, iterations) { }

#define BilateralFilter_BENCHMARK(ctype, dataType)                                                     \
BENCHMARK_F( BilateralFilter, BilateralFilter_##dataType, Fixture_2D_##dataType, samples, iterations)\
{                                                                                                      \
    array B = bilateral(A, 2.5f, 50.0f);                                                               \
    B.eval();                                                                                          \
}                                                                                                      \

BilateralFilter_BENCHMARK(uint8_t, u8)
BilateralFilter_BENCHMARK(int16_t, s16)
BilateralFilter_BENCHMARK(int32_t, s32)
BilateralFilter_BENCHMARK(long long, s64)
BilateralFilter_BENCHMARK(float, f32)
BilateralFilter_BENCHMARK(double, f64)
