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

CONVOLVE_BASELINE (f32, 5, 5)
CONVOLVE_BENCHMARK(f32, 5, 5)
CONVOLVE_BASELINE (f32, 9, 9)
CONVOLVE_BENCHMARK(f32, 9, 9)
CONVOLVE_BASELINE (f32, 11, 11)
CONVOLVE_BENCHMARK(f32, 11, 11)

CONVOLVE_BASELINE (f64, 5, 5)
CONVOLVE_BENCHMARK(f64, 5, 5)
CONVOLVE_BASELINE (f64, 9, 9)
CONVOLVE_BENCHMARK(f64, 9, 9)
CONVOLVE_BASELINE (f64, 11, 11)
CONVOLVE_BENCHMARK(f64, 11, 11)

