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

BASELINE_F(Resize, Baseline, Fixture_2D_f32, samples, operations)
{

}


BENCHMARK_F(Resize, f32_INTERP_NEAREST, Fixture_2D_f32, samples, operations)
{
	dim4 A_size = this->A.dims();
	array B = resize(A, A_size.dims[0] / 2, A_size.dims[1] / 2, AF_INTERP_NEAREST);
	B.eval();
}

BENCHMARK_F(Resize, f32_AF_INTERP_LINEAR, Fixture_2D_f32, samples, operations)
{
	dim4 A_size = A.dims();
	array B = resize(A, A_size.dims[0] / 2, A_size.dims[1] / 2, AF_INTERP_LINEAR);
	B.eval();
}

BENCHMARK_F(Resize, f32_INTERP_BILINEAR, Fixture_2D_f32, samples, operations)
{
	dim4 A_size = A.dims();
	array B = resize(A, A_size.dims[0] / 2, A_size.dims[1] / 2, AF_INTERP_BILINEAR);
	B.eval();
}

BENCHMARK_F(Resize, f32_INTERP_CUBIC, Fixture_2D_f32, samples, operations)
{
	dim4 A_size = A.dims();
	array B = resize(A, A_size.dims[0] / 2, A_size.dims[1] / 2, AF_INTERP_CUBIC);
	B.eval();
}

BENCHMARK_F(Resize, f64_INTERP_NEAREST, Fixture_2D_f64, samples, operations)
{
	dim4 A_size = this->A.dims();
	array B = resize(A, A_size.dims[0] / 2, A_size.dims[1] / 2, AF_INTERP_NEAREST);
	B.eval();
}

BENCHMARK_F(Resize, f64_AF_INTERP_LINEAR, Fixture_2D_f64, samples, operations)
{
	dim4 A_size = A.dims();
	array B = resize(A, A_size.dims[0] / 2, A_size.dims[1] / 2, AF_INTERP_LINEAR);
	B.eval();
}

BENCHMARK_F(Resize, f64_INTERP_BILINEAR, Fixture_2D_f64, samples, operations)
{
	dim4 A_size = A.dims();
	array B = resize(A, A_size.dims[0] / 2, A_size.dims[1] / 2, AF_INTERP_BILINEAR);
	B.eval();
}

BENCHMARK_F(Resize, f64_INTERP_CUBIC, Fixture_2D_f64, samples, operations)
{
	dim4 A_size = A.dims();
	array B = resize(A, A_size.dims[0] / 2, A_size.dims[1] / 2, AF_INTERP_CUBIC);
	B.eval();
}
