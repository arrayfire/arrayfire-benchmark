/*
 * convolve.cpp
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

BASELINE_F(Convolve_f32, Baseline, Fixture_2D_f32, samples, operations)
{
	// time the creation of the random 5x5 array
	array K = randu(5, 5, f32);
	K.eval();
	af::sync();
}

BENCHMARK_F(Convolve_f32, 2D_5x5, Fixture_2D_f32, samples, operations)
{
	array K = randu(5, 5, f32);
	K.eval();
    array B = convolve(this->A, K);
    B.eval();
	af::sync();
}

BASELINE_F(Convolve_f64, Baseline, Fixture_2D_f64, samples, operations)
{
	// time the creation of the random 5x5 array
	array K = randu(5, 5, f64);
	K.eval();
	af::sync();
}

BENCHMARK_F(Convolve_f64, 2D_5x5, Fixture_2D_f64, samples, operations)
{
	array K = randu(5, 5, f64);
	K.eval();
    array B = convolve(this->A, K);
    B.eval();
	af::sync();
}
