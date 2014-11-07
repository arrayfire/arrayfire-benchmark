/*
 * fft_2d.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: bkloppenborg
 */

#include <celero/Celero.h>
#include <arrayfire.h>
using namespace af;

extern unsigned int samples;
extern unsigned int operations;

class FFTFixture : public celero::TestFixture
{
public:
	array A_f32;
	array A_f64;

	FFTFixture()
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
		A_f32 = randu(experimentSize, experimentSize, f32);
		A_f64 = randu(experimentSize, experimentSize, f64);
	}
};

BASELINE_F(FFT_2D, f64, FFTFixture, samples, operations)
{

}

BENCHMARK_F(FFT_2D, f32, FFTFixture, samples, operations)
{
	array B = fft2(A_f32);
	B.eval();
}

BENCHMARK_F(FFT_2D, f64, FFTFixture, samples, operations)
{
	array B = fft2(A_f64);
	B.eval();
}
