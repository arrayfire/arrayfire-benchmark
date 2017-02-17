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

class AF_GFOR_Fixture : public AF_Fixture
{
public:
    af_dtype data_type;
    array input_cube;
    array output;

    AF_GFOR_Fixture() { this->data_type = af_dtype::f32; }

    virtual std::vector<std::pair<int64_t, uint64_t>> getExperimentValues() const
    {
        std::vector<std::pair<int64_t, uint64_t>> problemSpace;
        // 32 - 256 elements (2^5 - 2^8)
        for(int i = 5; i <= 8; i++)
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
            int depth = 100;
            input_cube = constant(1.0, experimentSize, experimentSize, depth, this->data_type);
            input_cube.eval();
            output = array(depth, this->data_type);
            output.eval();
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
BASELINE_F( GFOR, Baseline, AF_GFOR_Fixture, samples,  operations) {}


// Benchmark without using gfor:
BENCHMARK_F( GFOR , GFOR_NO_LOOP_SUM , AF_GFOR_Fixture , 1, 1) //samples, operations)
{
    output = sum(sum(input_cube));

    output.eval();
}

// Benchmark without using gfor:
BENCHMARK_F( GFOR , GFOR_FOR_LOOP_SUM , AF_GFOR_Fixture , 1, 1) //samples, operations)
{
    for(int i = 0; i < input_cube.dims(2); i++)
    {
        output(i) = sum(sum(input_cube(span, span, i)));
    }

    output.eval();;
}


// Benchmark using gfor:
BENCHMARK_F( GFOR , GFOR_SUM , AF_GFOR_Fixture , 1, 1) //samples, operations)
{
    gfor(seq ii, input_cube.dims(2))
    {
        output(ii) = sum(sum(input_cube(span, span, ii)));
    }

    output.eval();
}

