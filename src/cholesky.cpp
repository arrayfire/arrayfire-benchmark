/*
 * lapack_cholesky.cpp
 *
 *  Created on: February 29, 2016
 *      Author: syurkevi
 */

#include <celero/Celero.h>
#include <arrayfire.h>
#include "fixtures.h"
using namespace af;

extern unsigned int samples;
extern unsigned int operations;

class AF_Cholesky_Fixture : public AF_Fixture_2D
{
public:

    AF_Cholesky_Fixture() : AF_Fixture_2D() {}

    AF_Cholesky_Fixture(af_dtype data_type) : AF_Fixture_2D(data_type) {}

    /// Before each run, build a vector of random integers.
    virtual void setUp(int64_t experimentSize)
    {
        experimentSize = experimentSize <= 0 ? 1 : experimentSize;
        try
        {
            // We allocate square arrays with width = sqrt(experimentSize)
            unsigned int width = sqrt(experimentSize);
            deviceGC();
            int64_t n = width;
            af::array a = af::randu(n, n, this->data_type);
            af::array b = 10 * n * af::identity(n, n, this->data_type);
            A = matmul(a.H(), a) + b;
            A.eval();
            af::sync();
        }
        catch (af::exception & e)
        {
            // print out the error, rethrow the error to cause the test to fail.
            std::cout << e << std::endl;
            throw e;
        }
    }

};

class AF_Cholesky_Fixture_f32 : public AF_Cholesky_Fixture
{
public:
    AF_Cholesky_Fixture_f32() : AF_Cholesky_Fixture(af_dtype::f32) {}
};

class AF_Cholesky_Fixture_f64 : public AF_Cholesky_Fixture
{
public:
    AF_Cholesky_Fixture_f64() : AF_Cholesky_Fixture(af_dtype::f64) {}
};

BASELINE_F(LAPACK_Cholesky_f32, Baseline, AF_Cholesky_Fixture_f32, samples, operations) { }
BASELINE_F(LAPACK_Cholesky_f64, Baseline, AF_Cholesky_Fixture_f64, samples, operations) { }

BENCHMARK_F(LAPACK_Cholesky_f32, Cholesky_f32, AF_Cholesky_Fixture_f32, samples, operations)
{
    array out;
    cholesky(out, A);
    out.eval();
}

BENCHMARK_F(LAPACK_Cholesky_f64, Cholesky_f64, AF_Cholesky_Fixture_f64, samples, operations)
{
    array out;
    cholesky(out, A);
    out.eval();
}
