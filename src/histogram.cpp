/*
 * bandwidth.cpp
 *
 *  Created on: Nov 6, 2014
 *      Author: bkloppenborg
 */

#include <celero/Celero.h>
#include <arrayfire.h>
#include "fixtures.h"
using namespace af;

extern unsigned int samples;
extern unsigned int operations;


BASELINE_F(Histogram_f32, Baseline, Fixture_2D_f32, samples, operations)
{
	// rescale values to 0 -> 255 to correspond to bin ranges
    A *= 255;
    A.eval();
}

BENCHMARK_F(Histogram_f32, Histogram_f32, Fixture_2D_f32, samples, operations)
{
	// rescale values to 0 -> 255 to correspond to bin ranges
    A *= 255;
    A.eval();
	array B = histogram(A, 256, 0, 255);
	B.eval();
}


BASELINE_F(Histogram_f64, Baseline, Fixture_2D_f64, samples, operations)
{
	// rescale values to 0 -> 255 to correspond to bin ranges
    A *= 255;
    A.eval();
}

BENCHMARK_F(Histogram_f64, Histogram_f64, Fixture_2D_f64, samples, operations)
{
	// rescale values to 0 -> 255 to correspond to bin ranges
    A *= 255;
    A.eval();
	array B = histogram(A, 256, 0, 255);
	B.eval();
}
