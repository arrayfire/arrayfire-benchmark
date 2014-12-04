/*
 * matrix_multiply.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: bkloppenborg
 */

#include <math.h>
#include <celero/Celero.h>
#include <arrayfire.h>
#include "fixtures.h" // defines most test figures

using namespace af;

extern unsigned int samples;
extern unsigned int operations;

// Benchmarks for 32-bit floating point tests
BASELINE_F(Transpose_f32, Baseline, Fixture_2D_f32, samples, operations) { }
BENCHMARK_F(Transpose_f32, Benchmark, Fixture_2D_f32, samples, operations)
{
	array B = transpose(A);
	B.eval();
	af::sync();
}

// Benchmarks for 64-bit floating point tests
BASELINE_F(Transpose_f64, Baseline, Fixture_2D_f64, samples, operations) { }
BENCHMARK_F(Transpose_f64, Benchmark, Fixture_2D_f64, samples, operations)
{
	array B = transpose(A);
	B.eval();
	af::sync();
}
