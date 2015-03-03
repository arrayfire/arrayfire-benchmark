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
extern unsigned int operations;

#define CONVOLVE_BASELINE(dataType, kernelWidth, kernelHeight) \
BASELINE_F( Convolve_##kernelWidth##_##kernelHeight##_##dataType , \
    2D_##kernelWidth##_##kernelHeight, \
    Fixture_2D_##dataType , samples, operations) \
{                                                           \
    array K = randu( kernelWidth, kernelHeight , dataType); \
    K.eval();                                               \
    af::sync();                                             \
}                                                           \

#define CONVOLVE_BENCHMARK(dataType, kernelWidth, kernelHeight) \
BENCHMARK_F( Convolve_##kernelWidth##_##kernelHeight##_##dataType , \
    2D_##kernelWidth##_##kernelHeight , \
    Fixture_2D_##dataType , samples, operations) \
{                                                         \
    array K = randu(kernelWidth, kernelHeight, dataType); \
    K.eval();                                             \
    array B = convolve(this->A, K);                       \
    B.eval();                                             \
    af::sync();                                           \
}                                                         \

CONVOLVE_BASELINE (f32, 5, 5);
CONVOLVE_BENCHMARK(f32, 5, 5);
CONVOLVE_BASELINE (f32, 9, 9);
CONVOLVE_BENCHMARK(f32, 9, 9);
CONVOLVE_BASELINE (f32, 11, 11);
CONVOLVE_BENCHMARK(f32, 11, 11);

CONVOLVE_BASELINE (f64, 5, 5);
CONVOLVE_BENCHMARK(f64, 5, 5);
CONVOLVE_BASELINE (f64, 9, 9);
CONVOLVE_BENCHMARK(f64, 9, 9);
CONVOLVE_BASELINE (f64, 11, 11);
CONVOLVE_BENCHMARK(f64, 11, 11);

