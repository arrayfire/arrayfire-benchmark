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
BASELINE_F(Bandwidth, Baseline, Fixture_2D_f32, samples, operations){}

BENCHMARK_F(Bandwidth, Bandwidth_f32, Fixture_2D_f32, samples, operations)
{
    array B = this->A.copy();
    B.eval();
}

BENCHMARK_F(Bandwidth, Bandwidth_f64, Fixture_2D_f64, samples, operations)
{
    array B = this->A.copy();
    B.eval();
}
