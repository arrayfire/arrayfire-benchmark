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

class AF_Cholesky_Fixture : public AF_Fixture
{
public:
    af_dtype data_type;
    array A;
    int max_pow;

    AF_Cholesky_Fixture()
    {
        this->data_type = af_dtype::f32;
        max_pow = 14;
    }

    AF_Cholesky_Fixture(af_dtype data_type)
    {
        this->data_type = data_type;
        switch(this->data_type)
        {
            case f32: max_pow = 14; break;  // 32M *  4B = 128MB
            case f64: max_pow = 13; break;  // 16M *  8B = 128MB
            case c32: max_pow = 13; break;  // 16M *  8B = 128MB
            case c64: max_pow = 12; break;  //  8M * 16B = 128MB
            default : max_pow = 14; break;  // 32M *  4B = 128MB
        }
    }

    virtual std::vector<std::pair<int64_t, uint64_t>> getExperimentValues() const
    {
        std::vector<std::pair<int64_t, uint64_t>> problemSpace;
        // 256 - 4096 elements (2^8 - 2^12)
        // 256 - 16384 elements (2^8 - 2^14)
        for(int i = 5; i <= max_pow; i++)
        {
            auto experiment_size = std::make_pair<int64_t, uint64_t>(pow(2, i), 0);
            problemSpace.push_back(experiment_size);

        }

        return problemSpace;

    }

    /// Before each run, build a vector of random integers.
    virtual void setUp(int64_t experimentSize)
    {
        try
        {
            deviceGC();
            int64_t n = experimentSize;
            af::array a = af::randu(n, n, this->data_type);
            af::array b = 10 * n * af::identity(n, n, this->data_type);
            af::array A = matmul(a.H(), a) + b;
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

BASELINE_F(LAPACK_Cholesky, Baseline, Fixture_2D_f32, samples, operations) { }

BENCHMARK_F(LAPACK_Cholesky, Cholesky_f32, Fixture_2D_f32, samples, operations)
{
    array out;
    cholesky(out, A);
    out.eval();
}

BENCHMARK_F(LAPACK_Cholesky, Cholesky_f64, Fixture_2D_f64, samples, operations)
{
    array out;
    cholesky(out, A);
    out.eval();
}
