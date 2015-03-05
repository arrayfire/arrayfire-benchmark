/*
 * matrix_multiply.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: bkloppenborg
 */

#include <celero/Celero.h>
#include <arrayfire.h>
#include "fixtures.h" // defines most test figures

using namespace af;

extern unsigned int samples;
extern unsigned int operations;

// Benchmarks for 32-bit floating point tests
BASELINE_F(Accumulate, Baseline, Fixture_1D_f32, samples, operations) { }
BENCHMARK_F(Accumulate, Accumulate_1D_f32, Fixture_1D_f32, samples, operations)
{
	array B = accum(A);
	B.eval();
	af::sync();
}

BENCHMARK_F(Accumulate, Accumulate_2D_f32, Fixture_2D_f32, samples, operations)
{
	array B = accum(A);
	B.eval();
	af::sync();
}

// Benchmarks for 64-bit floating point tests
BENCHMARK_F(Accumulate, Accumulate_1D_f64, Fixture_1D_f64, samples, operations)
{
	array B = accum(A);
	B.eval();
	af::sync();
}

BENCHMARK_F(Accumulate, Accumulate_2D_f64, Fixture_2D_f64, samples, operations)
{
	array B = accum(A);
	B.eval();
	af::sync();
}
