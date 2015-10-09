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
extern unsigned int operations;

class AF_JIT_Fixture : public AF_Fixture
{
public:
    af_dtype data_type;
    array A;
    array B;
    array C;

    AF_JIT_Fixture() { this->data_type = af_dtype::f32; }

    virtual std::vector<std::pair<int64_t, uint64_t>> getExperimentValues() const
    {
        std::vector<std::pair<int64_t, uint64_t>> problemSpace;
        // 256 - 1048576 elements (2^8 - 2^20)
        // 256 - 33554432 elements (2^8 - 2^25)
        for(int i = 8; i <= 25; i++)
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

// do-nothing baseline measurement
BASELINE_F( JIT, Baseline, AF_JIT_Fixture, samples,  operations) {}

#define JIT_BENCHMARK(functionName, operation , fixture ) \
BENCHMARK_F( JIT , JIT_##functionName , fixture , samples, operations) \
{   \
    af::array result = operation ;  \
    result.eval();                  \
}   \

//
// Benchmark ArrayFire JIT operations when combining results:
//

// A few functions that combine results
JIT_BENCHMARK(AXPY,     A * B + C, AF_JIT_Fixture)
JIT_BENCHMARK(TRIGADD,  af::sin(A) + af::cos(B), AF_JIT_Fixture)
JIT_BENCHMARK(TRIGMULT, B * af::cos(A), AF_JIT_Fixture)
JIT_BENCHMARK(TRIGDIV,  af::cos(A) / B, AF_JIT_Fixture)

// Lastly a few interesting cases

// JIT with no function calls
BENCHMARK_F( JIT , JIT_NO_FUNCTION_CALL , AF_JIT_Fixture , samples, operations)
{
    af::array result = (A + B) + (A - B);
    result.eval();
}

// JIT with function calls
af::array jit_add(af::array A, af::array B) { return A + B; }
af::array jit_subtract(af::array A, af::array B) { return A - B; }

BENCHMARK_F( JIT , JIT_FUNCTION_CALL , AF_JIT_Fixture , samples, operations)
{
    af::array result = jit_add(A, B) + jit_subtract(A, B);
    result.eval();
}

//
// NOJIT
// Benchmark ArrayFire JIT operations when combining results but evaluating at
// every step
//

// do-nothing baseline measurement
BASELINE_F( NOJIT, Baseline, AF_JIT_Fixture, samples,  operations) {}

//
// Benchmark ArrayFire JIT operations when combining results:
//

// A few functions that combine results
BENCHMARK_F( NOJIT , NOJIT_AXPY , AF_JIT_Fixture , samples, operations)
{
    af::array temp1 = A + B;
    temp1.eval();
    af::array temp2 = temp1 + C;
    temp2.eval();
}

BENCHMARK_F( NOJIT , NOJIT_TRIGADD , AF_JIT_Fixture , samples, operations)
{
    af::array temp1 = af::sin(A);
    temp1.eval();
    af::array temp2 = af::cos(B);
    temp2.eval();
    af::array temp3 = temp1 + temp2;
    temp3.eval();
}

BENCHMARK_F( NOJIT , NOJIT_TRIGMULT , AF_JIT_Fixture , samples, operations)
{
    af::array temp1 = af::cos(A);
    temp1.eval();
    af::array temp2 = B * temp1;
    temp2.eval();
}

BENCHMARK_F( NOJIT , NOJIT_TRIGDIV , AF_JIT_Fixture , samples, operations)
{
    af::array temp1 = af::cos(A);
    temp1.eval();
    af::array temp2 = temp1 / B;
    temp2.eval();
}

// Lastly a few interesting cases

// NOJIT with no function calls
BENCHMARK_F( NOJIT , NOJIT_NO_FUNCTION_CALL , AF_JIT_Fixture , samples, operations)
{
    af::array temp1 = A + B;
    temp1.eval();
    af::array temp2 = A - B;
    temp2.eval();
    af::array result = temp1 + temp2;
    result.eval();
}

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

BENCHMARK_F( NOJIT , NOJIT_FUNCTION_CALL , AF_JIT_Fixture , samples, operations)
{
    af::array result = nojit_add(A, B) + nojit_subtract(A, B);
    result.eval();
}
