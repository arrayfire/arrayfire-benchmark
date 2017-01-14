/*
 * lapack_lu.cpp
 *
 *  Created on: February 29, 2016
 *      Author: syurkevi
 */

#include <celero/Celero.h>
#include <arrayfire.h>
#include "fixtures.h"
using namespace af;

extern unsigned int samples;
extern unsigned int iterations;


BASELINE_F(LAPACK_LU_f32, Baseline, AF_Fixture_2D, samples, iterations) { }
BASELINE_F(LAPACK_LU_f64, Baseline, AF_Fixture_2D, samples, iterations) { }

BENCHMARK_F(LAPACK_LU_f32, LU_f32, Fixture_2D_f32, samples, iterations)
{
    array pivot, l, u;
    lu(l, u, pivot, A);
}

BENCHMARK_F(LAPACK_LU_f64, LU_f64, Fixture_2D_f64, samples, iterations)
{
    array pivot, l, u;
    lu(l, u, pivot, A);
}
