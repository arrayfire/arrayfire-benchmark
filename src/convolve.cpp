/*
 * convolve.cpp
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

#define CONVOLVE_BASELINE(dataType, kernelWidth, kernelHeight)      \
BASELINE_F( Convolve_##dataType##_##kernelWidth##x##kernelHeight ,  \
            Baseline , Fixture_2D_##dataType , samples, iterations) \
{                                                                   \
    array K = randu( kernelWidth, kernelHeight , dataType);         \
    K.eval();                                                       \
}                                                                   \

#define CONVOLVE_BENCHMARK(dataType, kernelWidth, kernelHeight)     \
BENCHMARK_F( Convolve_##dataType##_##kernelWidth##x##kernelHeight , \
  Convolve_##dataType##_##kernelWidth##x##kernelHeight ,            \
    Fixture_2D_##dataType , samples, iterations)                    \
{                                                                   \
    array K = randu(kernelWidth, kernelHeight, dataType);           \
    K.eval();                                                       \
    array B = convolve(this->A, K);                                 \
    B.eval();                                                       \
}                                                                   \

#define CONVOLVE_BENCHMARK_PAIR(dataType, kernelWidth, kernelHeight) \
CONVOLVE_BASELINE (dataType, kernelWidth, kernelHeight)             \
CONVOLVE_BENCHMARK(dataType, kernelWidth, kernelHeight)             \

#define CONVOLVE_BENCHMARK_SET(dataType)                            \
CONVOLVE_BENCHMARK_PAIR(dataType, 5, 5)                             \
CONVOLVE_BENCHMARK_PAIR(dataType, 9, 9)                             \
CONVOLVE_BENCHMARK_PAIR(dataType, 11, 11)                           \


CONVOLVE_BENCHMARK_SET(u8)
CONVOLVE_BENCHMARK_SET(s16)
CONVOLVE_BENCHMARK_SET(s32)
CONVOLVE_BENCHMARK_SET(s64)
CONVOLVE_BENCHMARK_SET(f32)
CONVOLVE_BENCHMARK_SET(f64)
