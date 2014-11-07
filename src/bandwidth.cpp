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

class BandwidthFixture : public celero::TestFixture
{
public:
	array A;

	BandwidthFixture()
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
		A = constant(1, experimentSize, experimentSize);
	}
};

BASELINE_F(Bandwidth, CopyTest, BandwidthFixture, samples, operations)
{

}

BENCHMARK_F(Bandwidth, CopyTest, BandwidthFixture, samples, operations)
{
#warning Bandwidth.CopyTest is broken due to missing features in ArrayFire
	array B = this->A.copy();
	B.eval();
}

