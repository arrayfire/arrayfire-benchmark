/*
 * fft_2d.cpp
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

BASELINE_F(FFT_2D, Baseline, Fixture_2D_f32, samples, operations){ }

BENCHMARK_F(FFT_2D, FFT_2D_f32, Fixture_2D_f32, samples, operations)
{
    array B = fft2(A);
    B.eval();
}

BENCHMARK_F(FFT_2D, FFT_2D_f64, Fixture_2D_f64, samples, operations)
{
    array B = fft2(A);
    B.eval();
}
