/*
 * bandwidth.cpp
 *
 *  Created on: Nov 6, 2014
 *      Author: bkloppenborg
 */

#include <celero/Celero.h>
#include <arrayfire.h>
using namespace af;

extern unsigned int samples;
extern unsigned int operations;

class HistogramFixture : public celero::TestFixture
{
public:
	array A;

	HistogramFixture()
	{

	}

	virtual std::vector<int64_t> getExperimentValues() const
	{
		std::vector<int64_t> problemSpace;

		const int totalNumberOfTests = 10;

		for(int i = 0; i < totalNumberOfTests; i++)
		{
			// ExperimentValues is part of the base class and allows us to specify
			// some values to control various test runs to end up building a nice graph.
			problemSpace.push_back(512 * (i + 1));
		}

		return problemSpace;
	}

	/// Before each run, build a vector of random integers.
	virtual void setUp(int64_t experimentSize)
	{
		A = randu(experimentSize, experimentSize);
	}
};

BASELINE_F(Histogram, Baseline, HistogramFixture, samples, operations)
{

}

BENCHMARK_F(Histogram, Benchmark, HistogramFixture, samples, operations)
{
	array B = histogram(A,256,0,255);
	B.eval();
}

