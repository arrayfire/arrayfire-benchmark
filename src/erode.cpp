/*
 * erode.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: bkloppenborg
 */

#include <celero/Celero.h>
#include <arrayfire.h>
using namespace af;

extern unsigned int samples;
extern unsigned int operations;

class ErodeFixture : public celero::TestFixture
{
public:
	array A;

	ErodeFixture()
	{

	}

	virtual std::vector<int64_t> getExperimentValues() const
	{
		std::vector<int64_t> problemSpace;

		for(int i = 5; i < 12; i++)
		{
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

BASELINE_F(Erode, 5x5, ErodeFixture, samples, operations)
{
	// time the creation of the random 5x5 array
	array K = constant(1, 3, 3);
}

BENCHMARK_F(Erode, 5x5, ErodeFixture, samples, operations)
{
	array K = constant(1, 3, 3);
    array B = erode(this->A, K);
    B.eval();
}
