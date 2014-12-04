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

// 32-bit functions
BASELINE_F(Bandwidth_f32, Baseline, Fixture_2D_f32, samples, operations){}

BENCHMARK_F(Bandwidth_f32, CopyTest, Fixture_2D_f32, samples, operations)
{
#warning Bandwidth.CopyTest is broken due to missing features in ArrayFire
//	array B = this->A.copy();
//	B.eval();
//	af::sync();
}

// 64-bit functions
BASELINE_F(Bandwidth_f64, Baseline, Fixture_2D_f64, samples, operations){}

BENCHMARK_F(Bandwidth_f64, CopyTest, Fixture_2D_f64, samples, operations)
{
#warning Bandwidth.CopyTest is broken due to missing features in ArrayFire
//	array B = this->A.copy();
//	B.eval();
//	af::sync();
}
