/*
 * fixtures.h
 *
 *  Created on: Nov 11, 2014
 *      Author: bkloppenborg
 */

#ifndef SRC_FIXTURES_H_
#define SRC_FIXTURES_H_

#include <iostream>
#include <arrayfire.h>
using namespace af;

class AF_Fixture : public celero::TestFixture
{
public:
    std::vector<std::pair<int64_t, uint64_t>> problemSpace;
    static std::vector<std::pair<int64_t, uint64_t>> overrideProblemSpace;
    static bool use_max_problemspace;

    void onExperimentEnd()
    {
        af::sync();
    }
    virtual std::vector<std::pair<int64_t, uint64_t>> getExperimentValues() const
    {
        if(overrideProblemSpace.size()){
            return overrideProblemSpace;
        }
        return problemSpace;
    }
};

// Base class for all ArrayFire 1D fixtures
class AF_Fixture_1D : public AF_Fixture
{
public:
    af_dtype data_type;
    array A;
    int max_pow;

    AF_Fixture_1D()
    {
        this->data_type = af_dtype::f32;
        max_pow = 25;
        setDefaultExperimentValues();
    }
    AF_Fixture_1D(af_dtype data_type)
    {
        this->data_type = data_type;
        switch(this->data_type)
        {
            case f32: max_pow = 24; break;  // 16M *  4B = 64MB
            case f64: max_pow = 24; break;  // 16M *  8B = 128MB
            case c32: max_pow = 23; break;  // 16M *  8B = 128MB
            case c64: max_pow = 23; break;  // 16M * 16B = 256MB
            default : max_pow = 24; break;  // 32M *  4B = 64MB
        }
        setDefaultExperimentValues();
    }

    void setDefaultExperimentValues()
    {
        // 256 - 1048576 elements (2^8 - 2^20)
        // 256 - 33554432 elements (2^8 - 2^25)
        this->problemSpace.clear();
        for(int i = 8; i <= max_pow; i++)
        {
            auto experiment_size = std::make_pair<int64_t, int64_t>((int64_t)pow(2, i), (int64_t)0);
            this->problemSpace.push_back(experiment_size);
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

// 32 and 64-bit 1D fixtures

template<typename T>
class Fixture_1D : public AF_Fixture_1D
{
public:
    Fixture_1D() : AF_Fixture_1D(af_dtype(dtype_traits<T>::af_type)) {}
};

// Base class for all ArrayFire 2D fixtures
class AF_Fixture_2D : public AF_Fixture
{
public:
    af_dtype data_type;
    array A;
    int max_pow;

    AF_Fixture_2D()
    {
        this->data_type = af_dtype::f32;
        max_pow = 25;
        setDefaultExperimentValues();
    }
    AF_Fixture_2D(af_dtype data_type)
    {
        this->data_type = data_type;
        switch(this->data_type)
        {
            case f32: max_pow = 12; break;  // 4K * 4K *  4B =  64MB
            case f64: max_pow = 12; break;  // 4K * 4K *  8B = 128MB
            case s64: max_pow = 12; break;  // 4K * 4K *  8B = 128MB
            case c32: max_pow = 11; break;  // 4K * 4K *  8B = 128MB
            case c64: max_pow = 11; break;  // 4K * 4K * 16B = 256MB
            default : max_pow = 12; break;  // 4K * 4K *  4B = 128MB
        }
        setDefaultExperimentValues();
    }

    void setDefaultExperimentValues()
    {
        this->problemSpace.clear();
        // 32 x 32 - 32768 x 32768 (2^5 - 2^15)
        // 32 x 32 - 8196 x 8196 (2^5 - 2^13)
        for(int i = 5; i < max_pow; i++)
        {
            // create square problems that are equal in size to the linear
            // memory allocation.
            unsigned int width = pow(2, i);
            unsigned int problem_size = pow(width, 2);
            auto experiment_size = std::make_pair<int64_t, int64_t>((int64_t)problem_size, (int64_t)0);
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
        // We allocate square arrays with width = sqrt(experimentSize)
        unsigned int width = sqrt(experimentSize);

        try
        {
            deviceGC();
            A = randu(width, width, this->data_type);
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

template<typename T>
class Fixture_2D : public AF_Fixture_2D
{
public:
    Fixture_2D() : AF_Fixture_2D(af_dtype(dtype_traits<T>::af_type)) {}
};

typedef Fixture_1D<uint8_t> Fixture_1D_u8;
typedef Fixture_1D<int16_t> Fixture_1D_s16;
typedef Fixture_1D<uint16_t> Fixture_1D_u16;
typedef Fixture_1D<int32_t> Fixture_1D_s32;
typedef Fixture_1D<uint32_t> Fixture_1D_u32;
typedef Fixture_1D<long long> Fixture_1D_s64;
typedef Fixture_1D<unsigned long long> Fixture_1D_u64;
typedef Fixture_1D<float> Fixture_1D_f32;
typedef Fixture_1D<double> Fixture_1D_f64;

typedef Fixture_2D<uint8_t> Fixture_2D_u8;
typedef Fixture_2D<int16_t> Fixture_2D_s16;
typedef Fixture_2D<uint16_t> Fixture_2D_u16;
typedef Fixture_2D<int32_t> Fixture_2D_s32;
typedef Fixture_2D<uint32_t> Fixture_2D_u32;
typedef Fixture_2D<long long> Fixture_2D_s64;
typedef Fixture_2D<unsigned long long> Fixture_2D_u64;
typedef Fixture_2D<float> Fixture_2D_f32;
typedef Fixture_2D<double> Fixture_2D_f64;


#endif /* SRC_FIXTURES_H_ */
