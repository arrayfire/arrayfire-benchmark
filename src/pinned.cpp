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

class PinnedFixture_2D_f32 : public Fixture_2D_f32
{
public:
	float * A_host;

	PinnedFixture_2D_f32() {}

	virtual void setUp(int64_t experimentSize)
	{
        A_host = af::pinned<float>(size_t(experimentSize));
	}

	virtual void tearDown()
	{
        af::freePinned(A_host);
	}
};

class PinnedFixture_2D_f64 : public Fixture_2D_f64
{
public:
	double * A_host;

	PinnedFixture_2D_f64() {}

	virtual void setUp(int64_t experimentSize)
	{
        A_host = af::pinned<double>(size_t(experimentSize));
	}

	virtual void tearDown()
	{
        af::freePinned(A_host);
	}
};

BASELINE_F(PinnedMemory, Baseline, PinnedFixture_2D_f32, samples, operations) { }

BENCHMARK_F(PinnedMemory, PinnedMemory_f32_Bandwidth, PinnedFixture_2D_f32, samples, operations)
{
    dim4 A_size = A.dims();
    array B = array(A_size.dims[0], A_size.dims[1], A_host, af::afHost);
    B.eval();
}

BENCHMARK_F(PinnedMemory, PinnedMemory_f64_Bandwidth, PinnedFixture_2D_f64, samples, operations)
{
    dim4 A_size = A.dims();
    array B = array(A_size.dims[0], A_size.dims[1], A_host, af::afHost);
    B.eval();
}

