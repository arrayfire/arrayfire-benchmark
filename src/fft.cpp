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
extern unsigned int iterations;

BASELINE_F(FFT_2D, Baseline, Fixture_2D_f32, samples, iterations){ }

BENCHMARK_F(FFT_2D, FFT_2D_f32, Fixture_2D_f32, samples, iterations)
{
    array B = fft2(A);
    B.eval();
}

BENCHMARK_F(FFT_2D, FFT_2D_f64, Fixture_2D_f64, samples, iterations)
{
    array B = fft2(A);
    B.eval();
}

BASELINE_F(FFT_1D, Baseline, Fixture_1D_f32, samples, iterations){ }

BENCHMARK_F(FFT_1D, FFT_1D_f32, Fixture_1D_f32, samples, iterations)
{
    array B = fft(A);
    B.eval();
}

BENCHMARK_F(FFT_1D, FFT_1D_f64, Fixture_1D_f64, samples, iterations)
{
    array B = fft(A);
    B.eval();
}
