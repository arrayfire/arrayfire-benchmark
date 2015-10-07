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

class AF_JIT_Fixture : public AF_Fixture
{
public:
    af_dtype data_type;
    array A;
    array B;
    array C;

	AF_JIT_Fixture() { this->data_type = af_dtype::f32; }

    virtual std::vector<std::pair<int64_t, uint64_t>> getExperimentValues() const
	{
		std::vector<std::pair<int64_t, uint64_t>> problemSpace;
		// 256 - 1048576 elements (2^8 - 2^20)
		// 256 - 33554432 elements (2^8 - 2^25)
		for(int i = 8; i <= 25; i++)
        {
            auto experiment_size = std::make_pair<int64_t, uint64_t>(pow(2, i), 0);
            problemSpace.push_back(experiment_size);

        }

        return problemSpace;

	}

	/// Before each run, build a vector of random integers.
	virtual void setUp(int64_t experimentSize)
	{
		try
		{
			A = randu(experimentSize, this->data_type);
			B = randu(experimentSize, this->data_type);
			C = randu(experimentSize, this->data_type);
			A.eval();
            B.eval();
            C.eval();
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
BASELINE_F( JIT, Baseline, AF_JIT_Fixture, samples,  operations) {}

#define JIT_BENCHMARK(functionName, operation , fixture ) \
BENCHMARK_F( JIT , JIT_##functionName , fixture , samples, operations) \
{   \
    af::array result = operation ;  \
    result.eval();                  \
}   \

//
// Benchmark ArrayFire functions individually:
//

// built-in functions
JIT_BENCHMARK(MIN, af::min(A, B), AF_JIT_Fixture)
JIT_BENCHMARK(MAX, af::max(A, B), AF_JIT_Fixture)
JIT_BENCHMARK(ATAN2, af::atan2(A, B), AF_JIT_Fixture)
JIT_BENCHMARK(HYPOT, af::hypot(A, B), AF_JIT_Fixture)
JIT_BENCHMARK(POW, af::pow(A, B), AF_JIT_Fixture)
JIT_BENCHMARK(REMAINDER, af::rem(A, B), AF_JIT_Fixture)
JIT_BENCHMARK(MODULO, af::mod(A, B), AF_JIT_Fixture)

// trig functions
JIT_BENCHMARK(SINE,     af::sin(A),    AF_JIT_Fixture)
JIT_BENCHMARK(COSINE,   af::cos(A),    AF_JIT_Fixture)
JIT_BENCHMARK(TANGENT,   af::tan(A),    AF_JIT_Fixture)
JIT_BENCHMARK(ARC_SINE,     af::asin(A),    AF_JIT_Fixture)
JIT_BENCHMARK(ARC_COSINE,   af::acos(A),    AF_JIT_Fixture)
JIT_BENCHMARK(ARC_TANGENT,   af::atan(A),    AF_JIT_Fixture)
JIT_BENCHMARK(HYPERBOLIC_SINE,     af::sinh(A),    AF_JIT_Fixture)
JIT_BENCHMARK(HYPERBOLIC_COSINE,   af::cosh(A),    AF_JIT_Fixture)
JIT_BENCHMARK(HYPERBOLIC_TANGENT,   af::tanh(A),    AF_JIT_Fixture)
JIT_BENCHMARK(HYPERBOLIC_ARC_SINE,     af::asinh(A),    AF_JIT_Fixture)
JIT_BENCHMARK(HYPERBOLIC_ARC_COSINE,   af::acosh(A),    AF_JIT_Fixture)
JIT_BENCHMARK(HYPERBOLIC_ARC_TANGENT,   af::atanh(A),    AF_JIT_Fixture)

// exponentals and logs
JIT_BENCHMARK(EXP, af::exp(A), AF_JIT_Fixture)
JIT_BENCHMARK(EXP_M1, af::expm1(A), AF_JIT_Fixture)
JIT_BENCHMARK(ERF, af::erf(A), AF_JIT_Fixture)
JIT_BENCHMARK(ERFC, af::erfc(A), AF_JIT_Fixture)
JIT_BENCHMARK(LOG_E, af::log(A), AF_JIT_Fixture)
JIT_BENCHMARK(LOG_1P, af::log1p(A), AF_JIT_Fixture)
JIT_BENCHMARK(LOG10, af::log10(A), AF_JIT_Fixture)

// other functions
JIT_BENCHMARK(SQRT, af::sqrt(A), AF_JIT_Fixture)
JIT_BENCHMARK(CBRT, af::cbrt(A), AF_JIT_Fixture)
JIT_BENCHMARK(IS_ZERO, af::iszero(A), AF_JIT_Fixture)
JIT_BENCHMARK(IS_INF, af::isInf(A), AF_JIT_Fixture)
JIT_BENCHMARK(IS_NAN, af::isNaN(A), AF_JIT_Fixture)
JIT_BENCHMARK(TGAMMA, af::tgamma(A), AF_JIT_Fixture)
JIT_BENCHMARK(LGAMMA, af::lgamma(A), AF_JIT_Fixture)

//
// Benchmark ArrayFire JIT operations when combining results:
//

// basic mathematical operators
JIT_BENCHMARK(ADD,      A + B,     AF_JIT_Fixture)
JIT_BENCHMARK(SUBTRACT, A - B,     AF_JIT_Fixture)
JIT_BENCHMARK(MULTIPLY, A * B,     AF_JIT_Fixture)
JIT_BENCHMARK(DIVIDE,   A / B,     AF_JIT_Fixture)
JIT_BENCHMARK(ADD_CONSTANT, A + 1, AF_JIT_Fixture)
JIT_BENCHMARK(SUBTRACT_CONSTANT, A - 1, AF_JIT_Fixture)
JIT_BENCHMARK(MULTIPY_CONSTANT, 2 * A, AF_JIT_Fixture)
JIT_BENCHMARK(DIVIDE_CONSTANT, A / 2, AF_JIT_Fixture)

// A few functions that combine results
JIT_BENCHMARK(AXPY,      A * B + C, AF_JIT_Fixture)
JIT_BENCHMARK(TRIGADD,  af::sin(A) + af::cos(B), AF_JIT_Fixture)
JIT_BENCHMARK(TRIGMULT, B * af::cos(A), AF_JIT_Fixture)
JIT_BENCHMARK(TRIGDIV,  af::cos(A) / B, AF_JIT_Fixture)

// Lastly a few interesting cases

// JIT with no function calls
BENCHMARK_F( JIT , JIT_NO_FUNCTION_CALL , AF_JIT_Fixture , samples, operations)
{
    af::array result = (A + B) + (A - B);
    result.eval();
}

// JIT with function calls
af::array jit_add(af::array A, af::array B) { return A + B; }
af::array jit_subtract(af::array A, af::array B) { return A - B; }

BENCHMARK_F( JIT , JIT_FUNCTION_CALL , AF_JIT_Fixture , samples, operations)
{
    af::array result = jit_add(A, B) + jit_subtract(A, B);
    result.eval();
}
