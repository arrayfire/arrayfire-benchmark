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

class AF_ELWISE_Fixture : public AF_Fixture
{
public:
    af_dtype data_type;
    array A;
    array B;
    array C;

    AF_ELWISE_Fixture() { this->data_type = af_dtype::f32; }

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
BASELINE_F( ELWISE, Baseline, AF_ELWISE_Fixture, samples,  operations) {}

#define ELWISE_BENCHMARK(functionName, operation , fixture ) \
BENCHMARK_F( ELWISE , ELWISE_##functionName , fixture , samples, operations) \
{   \
    af::array result = operation ;  \
    result.eval();                  \
}   \

//
// Benchmark ArrayFire functions individually:
//

// built-in functions
ELWISE_BENCHMARK(MIN, af::min(A, B), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(MAX, af::max(A, B), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(ATAN2, af::atan2(A, B), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(HYPOT, af::hypot(A, B), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(POW, af::pow(A, B), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(REMAINDER, af::rem(A, B), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(MODULO, af::mod(A, B), AF_ELWISE_Fixture)

// trig functions
ELWISE_BENCHMARK(SINE,     af::sin(A),    AF_ELWISE_Fixture)
ELWISE_BENCHMARK(COSINE,   af::cos(A),    AF_ELWISE_Fixture)
ELWISE_BENCHMARK(TANGENT,   af::tan(A),    AF_ELWISE_Fixture)
ELWISE_BENCHMARK(ARC_SINE,     af::asin(A),    AF_ELWISE_Fixture)
ELWISE_BENCHMARK(ARC_COSINE,   af::acos(A),    AF_ELWISE_Fixture)
ELWISE_BENCHMARK(ARC_TANGENT,   af::atan(A),    AF_ELWISE_Fixture)
ELWISE_BENCHMARK(HYPERBOLIC_SINE,     af::sinh(A),    AF_ELWISE_Fixture)
ELWISE_BENCHMARK(HYPERBOLIC_COSINE,   af::cosh(A),    AF_ELWISE_Fixture)
ELWISE_BENCHMARK(HYPERBOLIC_TANGENT,   af::tanh(A),    AF_ELWISE_Fixture)
ELWISE_BENCHMARK(HYPERBOLIC_ARC_SINE,     af::asinh(A),    AF_ELWISE_Fixture)
ELWISE_BENCHMARK(HYPERBOLIC_ARC_COSINE,   af::acosh(A),    AF_ELWISE_Fixture)
ELWISE_BENCHMARK(HYPERBOLIC_ARC_TANGENT,   af::atanh(A),    AF_ELWISE_Fixture)

// exponentals and logs
ELWISE_BENCHMARK(EXP, af::exp(A), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(EXP_M1, af::expm1(A), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(ERF, af::erf(A), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(ERFC, af::erfc(A), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(LOG_E, af::log(A), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(LOG_1P, af::log1p(A), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(LOG10, af::log10(A), AF_ELWISE_Fixture)

// other functions
ELWISE_BENCHMARK(SQRT, af::sqrt(A), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(CBRT, af::cbrt(A), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(IS_ZERO, af::iszero(A), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(IS_INF, af::isInf(A), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(IS_NAN, af::isNaN(A), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(TGAMMA, af::tgamma(A), AF_ELWISE_Fixture)
ELWISE_BENCHMARK(LGAMMA, af::lgamma(A), AF_ELWISE_Fixture)

// basic mathematical operators
ELWISE_BENCHMARK(ADD,      A + B,     AF_ELWISE_Fixture)
ELWISE_BENCHMARK(SUBTRACT, A - B,     AF_ELWISE_Fixture)
ELWISE_BENCHMARK(MULTIPLY, A * B,     AF_ELWISE_Fixture)
ELWISE_BENCHMARK(DIVIDE,   A / B,     AF_ELWISE_Fixture)
ELWISE_BENCHMARK(ADD_CONSTANT, A + 1, AF_ELWISE_Fixture)
ELWISE_BENCHMARK(SUBTRACT_CONSTANT, A - 1, AF_ELWISE_Fixture)
ELWISE_BENCHMARK(MULTIPY_CONSTANT, 2 * A, AF_ELWISE_Fixture)
ELWISE_BENCHMARK(DIVIDE_CONSTANT, A / 2, AF_ELWISE_Fixture)
