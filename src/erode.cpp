/*
 * erode.cpp
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

BASELINE_F(Erode_f32, 5x5, Fixture_2D_f32, samples, operations)
{
	// time the creation of the random 5x5 array
	array K = constant(1, 3, 3, f32);
}

BENCHMARK_F(Erode_f32, 5x5, Fixture_2D_f32, samples, operations)
{
	array K = constant(1, 3, 3, f32);
    array B = erode(this->A, K);
    B.eval();
}

BASELINE_F(Erode_f64, 5x5, Fixture_2D_f64, samples, operations)
{
	// time the creation of the random 5x5 array
	array K = constant(1, 3, 3, f64);
}

BENCHMARK_F(Erode_f64, 5x5, Fixture_2D_f64, samples, operations)
{
	array K = constant(1, 3, 3, f64);
    array B = erode(this->A, K);
    B.eval();
}
