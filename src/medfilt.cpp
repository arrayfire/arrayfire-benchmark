/*
 * matrix_multiply.cpp
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

BASELINE_F(MedianFilter, Baseline, Fixture_2D_f32, samples, iterations) { }


BENCHMARK_F(MedianFilter, MedianFilter_f32_5x5_PAD_ZERO, Fixture_2D_f32, samples, iterations)
{
    array B = medfilt(A, 5, 5, AF_PAD_ZERO);
    B.eval();
}

BENCHMARK_F(MedianFilter, MedianFilter_f32_5x5_PAD_SYM, Fixture_2D_f32, samples, iterations)
{
    array B = medfilt(A, 5, 5, AF_PAD_SYM);
    B.eval();
}

BENCHMARK_F(MedianFilter, MedianFilter_f64_5x5_PAD_ZERO, Fixture_2D_f64, samples, iterations)
{
    array B = medfilt(A, 5, 5, AF_PAD_ZERO);
    B.eval();
}

BENCHMARK_F(MedianFilter, MedianFilter_f64_5x5_PAD_SYM, Fixture_2D_f64, samples, iterations)
{
    array B = medfilt(A, 5, 5, AF_PAD_SYM);
    B.eval();
}
