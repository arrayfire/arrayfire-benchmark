/*
 * bilateral.cpp
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

BASELINE_F(BilateralFilter_f32, Baseline, Fixture_2D_f32, samples, operations){}

BENCHMARK_F(BilateralFilter_f32, Benchmark, Fixture_2D_f32, samples, operations)
{
	array B = bilateral(this->A, 2.5f, 50.0f);
	B.eval();
}

BASELINE_F(BilateralFilter_f64, Baseline, Fixture_2D_f64, samples, operations){}

BENCHMARK_F(BilateralFilter_f64, Benchmark, Fixture_2D_f64, samples, operations)
{
	array B = bilateral(this->A, 2.5f, 50.0f);
	B.eval();
}
