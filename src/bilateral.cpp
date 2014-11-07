/*
 * bilateral.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: bkloppenborg
 */

#include <celero/Celero.h>
#include <arrayfire.h>
using namespace af;

extern unsigned int samples;
extern unsigned int operations;

class BilateralFilterFixture : public celero::TestFixture
{
public:
	array A;

	BilateralFilterFixture()
	{

	}

	virtual std::vector<int64_t> getExperimentValues() const
	{
		std::vector<int64_t> problemSpace;

		for(int i = 5; i < 14; i++)
		{
			// ExperimentValues is part of the base class and allows us to specify
			// some values to control various test runs to end up building a nice graph.
			problemSpace.push_back(pow(2, i));
		}

		return problemSpace;
	}

	/// Before each run, build a vector of random integers.
	virtual void setUp(int64_t experimentSize)
	{
		A = randu(experimentSize, experimentSize);
	}
};

BASELINE_F(BilateralFilter, Benchmark, BilateralFilterFixture, samples, operations)
{

}

BENCHMARK_F(BilateralFilter, Benchmark, BilateralFilterFixture, samples, operations)
{
	array B = bilateral(this->A, 2.5f, 50.0f);
	B.eval();
}

