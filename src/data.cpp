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

class AF_Data_Fixture : public AF_Fixture
{
public:
    af_dtype data_type;
    array A;
    int max_pow;

    AF_Data_Fixture()
    {
        this->data_type = af_dtype::f32;
        max_pow = 25;
    }

    AF_Data_Fixture(af_dtype data_type)
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

class AF_Data_Fixture_f32 : public AF_Data_Fixture
{
public:
    AF_Data_Fixture_f32() : AF_Data_Fixture(af_dtype::f32) {}
};

class AF_Data_Fixture_f64 : public AF_Data_Fixture
{
public:
    AF_Data_Fixture_f64() : AF_Data_Fixture(af_dtype::f64) {}
};

// do-nothing baseline measurement
BASELINE_F( Data_f32, Baseline, AF_Data_Fixture_f32, samples,  operations) {}
BASELINE_F( Data_f64, Baseline, AF_Data_Fixture_f64, samples,  operations) {}

#define DATA_BENCHMARK(functionName, operation)                           \
BENCHMARK_F( Data_f32, Data_f32_##functionName, AF_Data_Fixture_f32,      \
        samples, operations)                                              \
{                                                                         \
    af::array result = operation(A.dims(), this->data_type) ;       \
    result.eval();                                                        \
}                                                                         \
BENCHMARK_F( Data_f64, Data_f64_##functionName, AF_Data_Fixture_f64,      \
        samples, operations)                                              \
{                                                                         \
    af::array result = operation(A.dims(), this->data_type) ;       \
    result.eval();                                                        \
}                                                                         \

DATA_BENCHMARK(RANDU    , af::randu)
DATA_BENCHMARK(RANDN    , af::randn)
DATA_BENCHMARK(IDENTITY , af::identity)
DATA_BENCHMARK(RANGE    , af::range)

BENCHMARK_F( Data_f32, Data_f32_CONSTANT, AF_Data_Fixture_f32, samples, operations)
{
    af::array result = af::constant(1, A.dims(), this->data_type);
    result.eval();
}
BENCHMARK_F( Data_f64, Data_f64_CONSTANT, AF_Data_Fixture_f64, samples, operations)
{
    af::array result = af::constant(1, A.dims(), this->data_type);
    result.eval();
}
