/*
 * erode.cpp
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

#define Erode_BASELINE(dataType, kernelWidth, kernelHeight)         \
BASELINE_F( Erode_##dataType##_##kernelWidth##x##kernelHeight ,     \
            Baseline , Fixture_2D_##dataType , samples, iterations) \
{                                                                   \
    array K = constant(1, kernelWidth, kernelHeight, dataType);     \
    K.eval();                                                       \
}                                                                   \

#define Erode_BENCHMARK(dataType, kernelWidth, kernelHeight)        \
BENCHMARK_F( Erode_##dataType##_##kernelWidth##x##kernelHeight ,    \
  Erode_##dataType##_##kernelWidth##x##kernelHeight ,               \
    Fixture_2D_##dataType , samples, iterations)                    \
{                                                                   \
    array K = constant(1, kernelWidth, kernelHeight, dataType);     \
    K.eval();                                                       \
    array B = erode(this->A, K);                                    \
    B.eval();                                                       \
}                                                                   \

#define Erode_BENCHMARK_PAIR(dataType, kernelWidth, kernelHeight) \
Erode_BASELINE (dataType, kernelWidth, kernelHeight)             \
Erode_BENCHMARK(dataType, kernelWidth, kernelHeight)             \

#define Erode_BENCHMARK_SET(dataType)                            \
Erode_BENCHMARK_PAIR(dataType, 3, 3)                             \
Erode_BENCHMARK_PAIR(dataType, 5, 5)                             \
Erode_BENCHMARK_PAIR(dataType, 9, 9)                             \
Erode_BENCHMARK_PAIR(dataType, 11, 11)                           \


Erode_BENCHMARK_SET(u8)
Erode_BENCHMARK_SET(s16)
Erode_BENCHMARK_SET(s32)
Erode_BENCHMARK_SET(s64)
Erode_BENCHMARK_SET(f32)
Erode_BENCHMARK_SET(f64)
