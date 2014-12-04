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
BASELINE_F(Resize_f32, Baseline, Fixture_2D_f32, samples, operations) { }

BENCHMARK_F(Resize_f32, INTERP_NEAREST, Fixture_2D_f32, samples, operations)
{
	dim4 A_size = this->A.dims();
	array B = resize(A, A_size.dims[0] / 2, A_size.dims[1] / 2, AF_INTERP_NEAREST);
	B.eval();
	af::sync();
}

BENCHMARK_F(Resize_f32, INTERP_LINEAR, Fixture_2D_f32, samples, operations)
{
#warning The AF_INTERP_LINEAR method has yet to be implemented. Enable this code when ArrayFire is ready
//	dim4 A_size = A.dims();
//	array B = resize(A, A_size.dims[0] / 2, A_size.dims[1] / 2, AF_INTERP_LINEAR);
//	B.eval();
}

BENCHMARK_F(Resize_f32, INTERP_BILINEAR, Fixture_2D_f32, samples, operations)
{
	dim4 A_size = A.dims();
	array B = resize(A, A_size.dims[0] / 2, A_size.dims[1] / 2, AF_INTERP_BILINEAR);
	B.eval();
	af::sync();
}

BENCHMARK_F(Resize_f32, INTERP_CUBIC, Fixture_2D_f32, samples, operations)
{
#warning The AF_INTERP_CUBIC method has yet to be implemented. Enable this code when ArrayFire is ready
//	dim4 A_size = A.dims();
//	array B = resize(A, A_size.dims[0] / 2, A_size.dims[1] / 2, AF_INTERP_CUBIC);
//	B.eval();
//	af::sync();
}

// Benchmarks for 64-bit floating point tests
BASELINE_F(Resize_f64, Baseline, Fixture_2D_f64, samples, operations) { }

BENCHMARK_F(Resize_f64, INTERP_NEAREST, Fixture_2D_f64, samples, operations)
{
	dim4 A_size = this->A.dims();
	array B = resize(A, A_size.dims[0] / 2, A_size.dims[1] / 2, AF_INTERP_NEAREST);
	B.eval();
	af::sync();
}

BENCHMARK_F(Resize_f64, AF_INTERP_LINEAR, Fixture_2D_f64, samples, operations)
{
#warning The AF_INTERP_LINEAR method has yet to be implemented. Enable this code when ArrayFire is ready
//	dim4 A_size = A.dims();
//	array B = resize(A, A_size.dims[0] / 2, A_size.dims[1] / 2, AF_INTERP_LINEAR);
//	B.eval();
//	af::sync();
}

BENCHMARK_F(Resize_f64, INTERP_BILINEAR, Fixture_2D_f64, samples, operations)
{
	dim4 A_size = A.dims();
	array B = resize(A, A_size.dims[0] / 2, A_size.dims[1] / 2, AF_INTERP_BILINEAR);
	B.eval();
	af::sync();
}

BENCHMARK_F(Resize_f64, INTERP_CUBIC, Fixture_2D_f64, samples, operations)
{
#warning The AF_INTERP_CUBIC method has yet to be implemented. Enable this code when ArrayFire is ready
//	dim4 A_size = A.dims();
//	array B = resize(A, A_size.dims[0] / 2, A_size.dims[1] / 2, AF_INTERP_CUBIC);
//	B.eval();
//	af::sync();
}
