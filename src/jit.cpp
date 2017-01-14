/*
 * jit.cpp
 *
 *  Created on: March 4, 2015
 *      Author: bkloppenborg
 */

#include <celero/Celero.h>
#include <arrayfire.h>
#include "fixtures.h"
using namespace af;

extern unsigned int samples;
extern unsigned int iterations;

class AF_JIT_Fixture : public AF_Fixture
{
public:
    af_dtype data_type;
    array A;
    array B;
    array C;
    int max_pow;

    AF_JIT_Fixture()
    {
        this->data_type = af_dtype::f32;
        max_pow = 25;
        setDefaultExperimentValues();
    }

    AF_JIT_Fixture(af_dtype data_type)
    {
        this->data_type = data_type;
        switch(this->data_type)
        {
            case f32: max_pow = 25; break;  // 32M *  4B = 128MB
            case f64: max_pow = 25; break;  // 16M *  8B = 128MB
            case c32: max_pow = 23; break;  // 16M *  8B = 128MB
            case c64: max_pow = 23; break;  //  8M * 16B = 128MB
            default : max_pow = 25; break;  // 32M *  4B = 128MB
        }
        setDefaultExperimentValues();
    }

    void setDefaultExperimentValues()
    {
        std::vector<std::pair<int64_t, uint64_t>> &problemSpace = this->problemSpace;
        // 256 - 1048576 elements (2^8 - 2^20)
        // 256 - 33554432 elements (2^8 - 2^25)
        for(int i = 8; i <= max_pow; i++)
        {
            auto experiment_size = std::make_pair<int64_t, uint64_t>(pow(2, i), 0);
            problemSpace.push_back(experiment_size);

        }

    }

    /// Before each run, build a vector of random integers.
    virtual void setUp(int64_t experimentSize)
    {
        experimentSize = experimentSize <= 0 ? 1 : experimentSize;
        try
        {
            deviceGC();
            A = randu(experimentSize, this->data_type);
            B = randu(experimentSize, this->data_type);
            C = randu(experimentSize, this->data_type);
            A.eval();
            B.eval();
            C.eval();
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

class AF_JIT_Fixture_f32 : public AF_JIT_Fixture
{
public:
    AF_JIT_Fixture_f32() : AF_JIT_Fixture(af_dtype::f32) {}
};

class AF_JIT_Fixture_f64 : public AF_JIT_Fixture
{
public:
    AF_JIT_Fixture_f64() : AF_JIT_Fixture(af_dtype::f64) {}
};

// do-nothing baseline measurement
BASELINE_F( JIT_f32, Baseline, AF_JIT_Fixture_f32, samples,  iterations) {}
BASELINE_F( JIT_f64, Baseline, AF_JIT_Fixture_f64, samples,  iterations) {}

#define JIT_BENCHMARK(functionName, operation)                      \
BENCHMARK_F(JIT_f32, JIT_f32_##functionName, AF_JIT_Fixture_f32,    \
            samples, iterations)                                    \
{                                                                   \
    af::array result = operation ;                                  \
    result.eval();                                                  \
}                                                                   \
BENCHMARK_F(JIT_f64, JIT_f64_##functionName, AF_JIT_Fixture_f64,    \
            samples, iterations)                                    \
{                                                                   \
    af::array result = operation ;                                  \
    result.eval();                                                  \
}                                                                   \

//
// Benchmark ArrayFire JIT iterations when combining results:
//

// A few functions that combine results
JIT_BENCHMARK(AXPY,     A * B + C)
JIT_BENCHMARK(TRIGADD,  af::sin(A) + af::cos(B))
JIT_BENCHMARK(TRIGMULT, B * af::cos(A))
JIT_BENCHMARK(TRIGDIV,  af::cos(A) / B)
JIT_BENCHMARK(NO_FUNCTION_CALL, (A + B) + (A - B))

// JIT with function calls
af::array jit_add(af::array A, af::array B) { return A + B; }
af::array jit_subtract(af::array A, af::array B) { return A - B; }

JIT_BENCHMARK(FUNCTION_CALL, jit_add(A, B) + jit_subtract(A, B))

//
// NOJIT
// Benchmark ArrayFire JIT iterations when combining results but evaluating at
// every step
//

// do-nothing baseline measurement
BASELINE_F( NOJIT_f32, Baseline, AF_JIT_Fixture_f32, samples,  iterations) {}
BASELINE_F( NOJIT_f64, Baseline, AF_JIT_Fixture_f64, samples,  iterations) {}

//
// Benchmark ArrayFire JIT iterations when combining results:
//

// NOJIT with function calls
af::array nojit_add(af::array A, af::array B)
{
    af::array ret = A + B;
    ret.eval();
    return ret;
}
af::array nojit_subtract(af::array A, af::array B)
{
    af::array ret = A - B;
    ret.eval();
    return ret;
}

// A few functions that combine results
#define NOJIT_BENCHMARK(type)                                                                       \
BENCHMARK_F( NOJIT_##type , NOJIT_##type##_AXPY , AF_JIT_Fixture_##type , samples, iterations)      \
{                                                                                                   \
    af::array temp1 = A * B;                                                                        \
    temp1.eval();                                                                                   \
    af::array temp2 = temp1 + C;                                                                    \
    temp2.eval();                                                                                   \
}                                                                                                   \
BENCHMARK_F( NOJIT_##type , NOJIT_##type##_TRIGADD , AF_JIT_Fixture_##type , samples, iterations)   \
{                                                                                                   \
    af::array temp1 = af::sin(A);                                                                   \
    temp1.eval();                                                                                   \
    af::array temp2 = af::cos(B);                                                                   \
    temp2.eval();                                                                                   \
    af::array temp3 = temp1 + temp2;                                                                \
    temp3.eval();                                                                                   \
}                                                                                                   \
BENCHMARK_F( NOJIT_##type , NOJIT_##type##_TRIGMULT , AF_JIT_Fixture_##type , samples, iterations)  \
{                                                                                                   \
    af::array temp1 = af::cos(A);                                                                   \
    temp1.eval();                                                                                   \
    af::array temp2 = B * temp1;                                                                    \
    temp2.eval();                                                                                   \
}                                                                                                   \
BENCHMARK_F( NOJIT_##type , NOJIT_##type##_TRIGDIV , AF_JIT_Fixture_##type , samples, iterations)   \
{                                                                                                   \
    af::array temp1 = af::cos(A);                                                                   \
    temp1.eval();                                                                                   \
    af::array temp2 = temp1 / B;                                                                    \
    temp2.eval();                                                                                   \
}                                                                                                   \
BENCHMARK_F( NOJIT_##type , NOJIT_##type##_NO_FUNCTION_CALL , AF_JIT_Fixture_##type , samples, iterations)  \
{                                                                                                   \
    af::array temp1 = A + B;                                                                        \
    temp1.eval();                                                                                   \
    af::array temp2 = A - B;                                                                        \
    temp2.eval();                                                                                   \
    af::array result = temp1 + temp2;                                                               \
    result.eval();                                                                                  \
}                                                                                                   \
BENCHMARK_F( NOJIT_##type , NOJIT_##type##_FUNCTION_CALL , AF_JIT_Fixture_##type , samples, iterations)     \
{                                                                                                   \
    af::array result = nojit_add(A, B) + nojit_subtract(A, B);                                      \
    result.eval();                                                                                  \
}                                                                                                   \

NOJIT_BENCHMARK(f32)
NOJIT_BENCHMARK(f64)
