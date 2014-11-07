/*
 * matrix_multiply.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: bkloppenborg
 */

#include <celero/Celero.h>
#include <arrayfire.h>
using namespace af;

extern unsigned int samples;
extern unsigned int operations;

class MatrixMultFixture : public celero::TestFixture
{
public:
	array A_f32;
	array A_f64;

	MatrixMultFixture()
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
		A_f32 = randu(experimentSize, experimentSize, f32);
		A_f64 = randu(experimentSize, experimentSize, f64);
	}
};

BASELINE_F(MatrixMultiply, Baseline, MatrixMultFixture, samples, operations)
{

}

BENCHMARK_F(MatrixMultiply, f32, MatrixMultFixture, samples, operations)
{
	array B = matmul(A_f32, A_f32);
	B.eval();
}

BENCHMARK_F(MatrixMultiply, f64, MatrixMultFixture, samples, operations)
{
	array B = matmul(A_f64, A_f64);
	B.eval();
}
