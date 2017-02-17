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
        setDefaultExperimentValues();
    }

    AF_ELWISE_Fixture(af_dtype data_type)
    {
        this->data_type = data_type;
        switch(this->data_type)
        {
            case f32: max_pow = 25; break;  // 32M *  4B = 128MB
            case f64: max_pow = 25; break;  // 32M *  8B = 256MB
            case c32: max_pow = 24; break;  // 32M *  8B = 256MB
            case c64: max_pow = 24; break;  // 32M * 16B = 512MB
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
        if(this->use_max_problemspace && problemSpace.size() > 1){
            problemSpace.erase(problemSpace.begin(), problemSpace.end() - 1);
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

template<typename T>
class ELWISE_Fixture : public AF_ELWISE_Fixture
{
public:
    ELWISE_Fixture() : AF_ELWISE_Fixture(af_dtype(dtype_traits<T>::af_type)) {}
};

typedef ELWISE_Fixture<uint8_t> AF_ELWISE_Fixture_u8;
typedef ELWISE_Fixture<int16_t> AF_ELWISE_Fixture_s16;
typedef ELWISE_Fixture<uint16_t> AF_ELWISE_Fixture_u16;
typedef ELWISE_Fixture<int32_t> AF_ELWISE_Fixture_s32;
typedef ELWISE_Fixture<uint32_t> AF_ELWISE_Fixture_u32;
typedef ELWISE_Fixture<long long> AF_ELWISE_Fixture_s64;
typedef ELWISE_Fixture<unsigned long long> AF_ELWISE_Fixture_u64;
typedef ELWISE_Fixture<float> AF_ELWISE_Fixture_f32;
typedef ELWISE_Fixture<double> AF_ELWISE_Fixture_f64;


// do-nothing baseline measurement
BASELINE_F( ELWISE_u8 , Baseline, AF_ELWISE_Fixture, samples,  iterations) {}
BASELINE_F( ELWISE_s16, Baseline, AF_ELWISE_Fixture, samples,  iterations) {}
BASELINE_F( ELWISE_s32, Baseline, AF_ELWISE_Fixture, samples,  iterations) {}
BASELINE_F( ELWISE_s64, Baseline, AF_ELWISE_Fixture, samples,  iterations) {}
BASELINE_F( ELWISE_f32, Baseline, AF_ELWISE_Fixture, samples,  iterations) {}
BASELINE_F( ELWISE_f64, Baseline, AF_ELWISE_Fixture, samples,  iterations) {}

#define _ELWISE_BENCHMARK(functionName, operation, ctype, dataType)                                                  \
BENCHMARK_F(ELWISE_##dataType, Elwise_##dataType##_##functionName, AF_ELWISE_Fixture_##dataType, samples, iterations)\
{                                                                                                                    \
    af::array result = operation ;                                                                                   \
    result.eval();                                                                                                   \
}                                                                                                                    \

#define ELWISE_BENCHMARK_REALS(functionName, operation)     \
    _ELWISE_BENCHMARK(functionName, operation, float, f32)  \
    _ELWISE_BENCHMARK(functionName, operation, double, f64) \

#define ELWISE_BENCHMARK(functionName, operation)           \
    _ELWISE_BENCHMARK(functionName, operation, uint8, u8)   \
    _ELWISE_BENCHMARK(functionName, operation, int16, s16)  \
    _ELWISE_BENCHMARK(functionName, operation, int32, s32)  \
    _ELWISE_BENCHMARK(functionName, operation, int64, s64)  \
    _ELWISE_BENCHMARK(functionName, operation, float, f32)  \
    _ELWISE_BENCHMARK(functionName, operation, double, f64) \


// Benchmark ArrayFire functions individually:
//

// built-in functions
ELWISE_BENCHMARK(MIN        , af::min(A, B))
ELWISE_BENCHMARK(MAX        , af::max(A, B))
ELWISE_BENCHMARK_REALS(ATAN2      , af::atan2(A, B))
ELWISE_BENCHMARK_REALS(HYPOT      , af::hypot(A, B))
ELWISE_BENCHMARK_REALS(POW        , af::pow(A, B))
ELWISE_BENCHMARK(REMAINDER  , af::rem(A, B))
ELWISE_BENCHMARK(MODULO     , af::mod(A, B))

// trig functions
ELWISE_BENCHMARK_REALS(SIN        , af::sin(A))
ELWISE_BENCHMARK_REALS(COS        , af::cos(A))
ELWISE_BENCHMARK_REALS(TAN        , af::tan(A))
ELWISE_BENCHMARK_REALS(ARC_SIN    , af::asin(A))
ELWISE_BENCHMARK_REALS(ARC_COS    , af::acos(A))
ELWISE_BENCHMARK_REALS(ARC_TAN    , af::atan(A))
ELWISE_BENCHMARK_REALS(HYP_SIN    , af::sinh(A))
ELWISE_BENCHMARK_REALS(HYP_COS    , af::cosh(A))
ELWISE_BENCHMARK_REALS(HYP_TAN    , af::tanh(A))
ELWISE_BENCHMARK_REALS(HYP_ARC_SIN, af::asinh(A))
ELWISE_BENCHMARK_REALS(HYP_ARC_COS, af::acosh(A))
ELWISE_BENCHMARK_REALS(HYP_ARC_TAN, af::atanh(A))

// exponentals and logs
ELWISE_BENCHMARK_REALS(EXP        , af::exp(A))
ELWISE_BENCHMARK_REALS(EXP_M1     , af::expm1(A))
ELWISE_BENCHMARK_REALS(ERF        , af::erf(A))
ELWISE_BENCHMARK_REALS(ERFC       , af::erfc(A))
ELWISE_BENCHMARK_REALS(LOG_E      , af::log(A))
ELWISE_BENCHMARK_REALS(LOG_1P     , af::log1p(A))
ELWISE_BENCHMARK_REALS(LOG10      , af::log10(A))

// other functions
ELWISE_BENCHMARK_REALS(SQRT       , af::sqrt(A))
ELWISE_BENCHMARK_REALS(CBRT       , af::cbrt(A))
ELWISE_BENCHMARK_REALS(IS_ZERO    , af::iszero(A))
ELWISE_BENCHMARK_REALS(IS_INF     , af::isInf(A))
ELWISE_BENCHMARK_REALS(IS_NAN     , af::isNaN(A))
ELWISE_BENCHMARK_REALS(TGAMMA     , af::tgamma(A))
ELWISE_BENCHMARK_REALS(LGAMMA     , af::lgamma(A))

// basic mathematical operators
ELWISE_BENCHMARK(ADD        , A + B)
ELWISE_BENCHMARK(SUBTRACT   , A - B)
ELWISE_BENCHMARK(MULTIPLY   , A * B)
ELWISE_BENCHMARK(DIVIDE     , A / B)
ELWISE_BENCHMARK(ADD_CONSTANT       , A + 1)
ELWISE_BENCHMARK(SUBTRACT_CONSTANT  , A - 1)
ELWISE_BENCHMARK(MULTIPY_CONSTANT   , 2 * A)
ELWISE_BENCHMARK(DIVIDE_CONSTANT    , A / 2)
