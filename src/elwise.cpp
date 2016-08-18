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
    int max_pow;

    AF_ELWISE_Fixture()
    {
        this->data_type = af_dtype::f32;
        max_pow = 25;
    }

    AF_ELWISE_Fixture(af_dtype data_type)
    {
        this->data_type = data_type;
        switch(this->data_type)
        {
            case f32: max_pow = 25; break;  // 32M *  4B = 128MB
            case f64: max_pow = 24; break;  // 16M *  8B = 128MB
            case c32: max_pow = 24; break;  // 16M *  8B = 128MB
            case c64: max_pow = 23; break;  //  8M * 16B = 128MB
            default : max_pow = 25; break;  // 32M *  4B = 128MB
        }
    }

    virtual std::vector<std::pair<int64_t, uint64_t>> getExperimentValues() const
    {
        std::vector<std::pair<int64_t, uint64_t>> problemSpace;
        // 256 - 1048576 elements (2^8 - 2^20)
        // 256 - 33554432 elements (2^8 - 2^25)
        for(int i = 8; i <= max_pow; i++)
        {
            auto experiment_size = std::make_pair<int64_t, uint64_t>(pow(2, i), 0);
            problemSpace.push_back(experiment_size);

        }
        if(this->use_max_problemspace && problemSpace.size() > 1){
            problemSpace.erase(problemSpace.begin(), problemSpace.end() - 1);
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
            A.eval();
            B.eval();
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

class AF_ELWISE_Fixture_f32 : public AF_ELWISE_Fixture
{
public:
    AF_ELWISE_Fixture_f32() : AF_ELWISE_Fixture(af_dtype::f32) {}
};

class AF_ELWISE_Fixture_f64 : public AF_ELWISE_Fixture
{
public:
    AF_ELWISE_Fixture_f64() : AF_ELWISE_Fixture(af_dtype::f64) {}
};

// do-nothing baseline measurement
BASELINE_F( ELWISE_f32, Baseline, AF_ELWISE_Fixture_f32, samples,  operations) {}
BASELINE_F( ELWISE_f64, Baseline, AF_ELWISE_Fixture_f64, samples,  operations) {}

#define ELWISE_BENCHMARK(functionName, operation)                               \
BENCHMARK_F( ELWISE_f32, ELWISE_f32_##functionName, AF_ELWISE_Fixture_f32,      \
        samples, operations)                                                    \
{                                                                               \
    af::array result = operation ;                                              \
    result.eval();                                                              \
}                                                                               \
BENCHMARK_F( ELWISE_f64, ELWISE_f64_##functionName, AF_ELWISE_Fixture_f64,      \
        samples, operations)                                                    \
{                                                                               \
    af::array result = operation ;                                              \
    result.eval();                                                              \
}                                                                               \

//
// Benchmark ArrayFire functions individually:
//

// built-in functions
ELWISE_BENCHMARK(MIN        , af::min(A, B))
ELWISE_BENCHMARK(MAX        , af::max(A, B))
ELWISE_BENCHMARK(ATAN2      , af::atan2(A, B))
ELWISE_BENCHMARK(HYPOT      , af::hypot(A, B))
ELWISE_BENCHMARK(POW        , af::pow(A, B))
ELWISE_BENCHMARK(REMAINDER  , af::rem(A, B))
ELWISE_BENCHMARK(MODULO     , af::mod(A, B))

// trig functions
ELWISE_BENCHMARK(SIN        , af::sin(A))
ELWISE_BENCHMARK(COS        , af::cos(A))
ELWISE_BENCHMARK(TAN        , af::tan(A))
ELWISE_BENCHMARK(ARC_SIN    , af::asin(A))
ELWISE_BENCHMARK(ARC_COS    , af::acos(A))
ELWISE_BENCHMARK(ARC_TAN    , af::atan(A))
ELWISE_BENCHMARK(HYP_SIN    , af::sinh(A))
ELWISE_BENCHMARK(HYP_COS    , af::cosh(A))
ELWISE_BENCHMARK(HYP_TAN    , af::tanh(A))
ELWISE_BENCHMARK(HYP_ARC_SIN, af::asinh(A))
ELWISE_BENCHMARK(HYP_ARC_COS, af::acosh(A))
ELWISE_BENCHMARK(HYP_ARC_TAN, af::atanh(A))

// exponentals and logs
ELWISE_BENCHMARK(EXP        , af::exp(A))
ELWISE_BENCHMARK(EXP_M1     , af::expm1(A))
ELWISE_BENCHMARK(ERF        , af::erf(A))
ELWISE_BENCHMARK(ERFC       , af::erfc(A))
ELWISE_BENCHMARK(LOG_E      , af::log(A))
ELWISE_BENCHMARK(LOG_1P     , af::log1p(A))
ELWISE_BENCHMARK(LOG10      , af::log10(A))

// other functions
ELWISE_BENCHMARK(SQRT       , af::sqrt(A))
ELWISE_BENCHMARK(CBRT       , af::cbrt(A))
ELWISE_BENCHMARK(IS_ZERO    , af::iszero(A))
ELWISE_BENCHMARK(IS_INF     , af::isInf(A))
ELWISE_BENCHMARK(IS_NAN     , af::isNaN(A))
ELWISE_BENCHMARK(TGAMMA     , af::tgamma(A))
ELWISE_BENCHMARK(LGAMMA     , af::lgamma(A))

// basic mathematical operators
ELWISE_BENCHMARK(ADD        , A + B)
ELWISE_BENCHMARK(SUBTRACT   , A - B)
ELWISE_BENCHMARK(MULTIPLY   , A * B)
ELWISE_BENCHMARK(DIVIDE     , A / B)
ELWISE_BENCHMARK(ADD_CONSTANT       , A + 1)
ELWISE_BENCHMARK(SUBTRACT_CONSTANT  , A - 1)
ELWISE_BENCHMARK(MULTIPY_CONSTANT   , 2 * A)
ELWISE_BENCHMARK(DIVIDE_CONSTANT    , A / 2)
