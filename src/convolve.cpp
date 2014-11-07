/*
 * convolve.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: bkloppenborg
 */

#include <celero/Celero.h>
#include <arrayfire.h>
using namespace af;

extern unsigned int samples;
extern unsigned int operations;

class ConvolveFixture : public celero::TestFixture
{
public:
	array A;

	ConvolveFixture()
	{

	}

	virtual std::vector<int64_t> getExperimentValues() const
	{
		std::vector<int64_t> problemSpace;

		for(int i = 5; i < 12; i++)
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

BASELINE_F(Convolve, 5x5, ConvolveFixture, samples, operations)
{
	// time the creation of the random 5x5 array
	array K = randu(5, 5);
}

BENCHMARK_F(Convolve, 5x5, ConvolveFixture, samples, operations)
{
	array K = randu(5, 5);
    array B = convolve(this->A, K);
    B.eval();
}
