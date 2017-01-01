/*
 * matrix_multiply.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: bkloppenborg
 */

#include <math.h>
#include <celero/Celero.h>
#include <arrayfire.h>
#include "fixtures.h" // defines most test figures

using namespace af;

extern unsigned int samples;
extern unsigned int iterations;

// Benchmarks for 32-bit floating point tests
BASELINE_F(Transpose, Baseline, Fixture_2D_f32, samples, iterations) { }
BENCHMARK_F(Transpose, Transpose_f32, Fixture_2D_f32, samples, iterations)
{
    array B = transpose(A);
    B.eval();
}

// Benchmarks for 64-bit floating point tests
BENCHMARK_F(Transpose, Transpose_f64, Fixture_2D_f64, samples, iterations)
{
    array B = transpose(A);
    B.eval();
}
