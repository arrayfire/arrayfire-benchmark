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
extern unsigned int operations;

BASELINE_F(MedianFilter_f32, Baseline, Fixture_2D_f32, samples, operations) { }


BENCHMARK_F(MedianFilter_f32, 4x4_PAD_ZERO, Fixture_2D_f32, samples, operations)
{
	array B = medfilt(A, 4, 4, af_pad_type::AF_ZERO);
	B.eval();
}

BENCHMARK_F(MedianFilter_f32, 4x4_PAD_SYMMETRIC, Fixture_2D_f32, samples, operations)
{
	array B = medfilt(A, 4, 4, af_pad_type::AF_SYMMETRIC);
	B.eval();
}

BASELINE_F(MedianFilter_f64, Baseline, Fixture_2D_f64, samples, operations) { }


BENCHMARK_F(MedianFilter_f64, 4x4_PAD_ZERO, Fixture_2D_f64, samples, operations)
{
	array B = medfilt(A, 4, 4, af_pad_type::AF_ZERO);
	B.eval();
}

BENCHMARK_F(MedianFilter_f64, 4x4_PAD_SYMMETRIC, Fixture_2D_f64, samples, operations)
{
	array B = medfilt(A, 4, 4, af_pad_type::AF_SYMMETRIC);
	B.eval();
}
