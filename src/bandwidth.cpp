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
extern unsigned int iterations;

// 32-bit functions
BASELINE_F(Bandwidth, Baseline, AF_Fixture_2D, samples, iterations){}

BENCHMARK_F(Bandwidth, Bandwidth_f32, Fixture_2D_f32, samples, iterations)
{
    array B = this->A.copy();
    B.eval();
}

BENCHMARK_F(Bandwidth, Bandwidth_f64, Fixture_2D_f64, samples, iterations)
{
    array B = this->A.copy();
    B.eval();
}

BENCHMARK_F(Bandwidth, Bandwidth_s32, Fixture_2D_s32, samples, iterations)
{
    array B = this->A.copy();
    B.eval();
}

BENCHMARK_F(Bandwidth, Bandwidth_s64, Fixture_2D_s64, samples, iterations)
{
    array B = this->A.copy();
    B.eval();
}

BENCHMARK_F(Bandwidth, Bandwidth_u8, Fixture_2D_u8, samples, iterations)
{
    array B = this->A.copy();
    B.eval();
}
