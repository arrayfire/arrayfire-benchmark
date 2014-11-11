/*
 * fixtures.h
 *
 *  Created on: Nov 11, 2014
 *      Author: bkloppenborg
 */

#ifndef SRC_FIXTURES_H_
#define SRC_FIXTURES_H_

#include <arrayfire.h>
using namespace af;

// Base class for all ArrayFire 1D fixtures
class AF_Fixture_1D : public celero::TestFixture
{
public:
	af_dtype data_type;
	array A;

	AF_Fixture_1D(){ this->data_type = af_dtype::f32; }
	AF_Fixture_1D(af_dtype data_type){ this->data_type = data_type; }

	virtual std::vector<int64_t> getExperimentValues() const
	{
		std::vector<int64_t> problemSpace;
		// 256 - 1048576 elements
		for(int i = 8; i < 21; i++)
			problemSpace.push_back(pow(2, i));

		return problemSpace;

	}

	/// Before each run, build a vector of random integers.
	virtual void setUp(int64_t experimentSize)
	{
		A = randu(experimentSize, this->data_type);
	}
};

// 32 and 64-bit 1D fixtures
class Fixture_1D_f32 : public AF_Fixture_1D
{
public:
	Fixture_1D_f32() : AF_Fixture_1D(af_dtype::f32) {}
};

class Fixture_1D_f64 : public AF_Fixture_1D
{
public:
	Fixture_1D_f64() : AF_Fixture_1D(af_dtype::f64) {}
};

// Base class for all ArrayFire 2D fixtures
class AF_Fixture_2D : public celero::TestFixture
{
public:
	af_dtype data_type;
	array A;

	AF_Fixture_2D(){ this->data_type = af_dtype::f32; }
	AF_Fixture_2D(af_dtype data_type){ this->data_type = data_type; }

	virtual std::vector<int64_t> getExperimentValues() const
	{
		std::vector<int64_t> problemSpace;
		// 32 x 32 - 32768 x 32768
		for(int i = 5; i < 15; i++)
			problemSpace.push_back(pow(2, i));

		return problemSpace;

	}

	/// Before each run, build a vector of random integers.
	virtual void setUp(int64_t experimentSize)
	{
		A = randu(experimentSize, experimentSize, this->data_type);
	}
};

class Fixture_2D_f32 : public AF_Fixture_2D
{
public:
	Fixture_2D_f32() : AF_Fixture_2D(af_dtype::f32) {}
};

class Fixture_2D_f64 : public AF_Fixture_2D
{
public:
	Fixture_2D_f64() : AF_Fixture_2D(af_dtype::f64) {}
};



#endif /* SRC_FIXTURES_H_ */
