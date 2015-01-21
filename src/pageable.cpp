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

class PageFixture_2D_f32 : public Fixture_2D_f32
{
public:
	float * A_host;

	PageFixture_2D_f32() {}

	virtual void setUp(int64_t experimentSize)
	{
//#warning af::array::host<float> is not implemented
		A_host = A.host<float>();
	}

	virtual void tearDown()
	{
#warning af::array::free is not implemented. Fix this later
//		array::free(A_host);
	}
};

class PageFixture_2D_f64 : public Fixture_2D_f64
{
public:
	double * A_host;

	PageFixture_2D_f64() {}

	virtual void setUp(int64_t experimentSize)
	{
//#warning af::array::host<double> is not implemented
		A_host = A.host<double>();
	}

	virtual void tearDown()
	{
#warning af::array::free is not implemented. Fix this later
//		array::free(A_host);
	}
};

BASELINE_F(Pageable_f32, Baseline, PageFixture_2D_f32, samples, operations) { }

BENCHMARK_F(Pageable_f32, Benchmark, PageFixture_2D_f32, samples, operations)
{
#warning This test does nothing as critical portions of ArrayFire are not implemented.
//	dim4 A_size = A.dims();
//    array B = array(A_size.dims[0], A_size.dims[1], A_host, af::afHost);
//    B.eval();
//	af::sync();
}

