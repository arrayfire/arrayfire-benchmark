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

	PageFixture_2D_f32() 
    {
        A_host = NULL;
    }

	virtual void setUp(int64_t experimentSize)
	{
        A_host = A.host<float>();
	}

	virtual void tearDown()
	{
        if(A_host) delete[] A_host;
	}
};

class PageFixture_2D_f64 : public Fixture_2D_f64
{
public:
	double * A_host;

	PageFixture_2D_f64() 
    {
        A_host = NULL;
    }

	virtual void setUp(int64_t experimentSize)
	{
		A_host = A.host<double>();
	}

	virtual void tearDown()
	{
        if(A_host) delete[] A_host;
	}
};

//BASELINE_F(Pageable_f32, Baseline, PageFixture_2D_f32, samples, operations) { }
//
//BENCHMARK_F(Pageable_f32, Benchmark, PageFixture_2D_f32, samples, operations)
//{
//    dim4 A_size = A.dims();
//    array B = array(A_size.dims[0], A_size.dims[1], A_host, af::afHost);
//    B.eval();
//    af::sync();
//}
//
//BASELINE_F(Pageable_f64, Baseline, PageFixture_2D_f64, samples, operations) { }
//
//BENCHMARK_F(Pageable_f64, Benchmark, PageFixture_2D_f64, samples, operations)
//{
//    dim4 A_size = A.dims();
//    array B = array(A_size.dims[0], A_size.dims[1], A_host, af::afHost);
//    B.eval();
//    af::sync();
//}
