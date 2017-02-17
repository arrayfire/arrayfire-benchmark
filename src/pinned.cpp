/*
 * matrix_multiply.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: bkloppenborg
 */

#include <celero/Celero.h>
#include <arrayfire.h>
#include "fixtures.h"
#include <cstdint>
using namespace af;

extern unsigned int samples;
extern unsigned int iterations;

BASELINE_F(PinnedMemory, Baseline, AF_Fixture_2D, samples, iterations) { }

#define PinnedMemory_BENCHMARK(ctype, dataType)                                                                   \
class PinnedFixture_2D_##dataType : public Fixture_2D_##dataType                                                  \
{                                                                                                                 \
public:                                                                                                           \
    ctype * A_host;                                                                                               \
    int64_t A_size;                                                                                               \
                                                                                                                  \
    PinnedFixture_2D_##dataType() {}                                                                              \
                                                                                                                  \
    virtual void setUp(int64_t experimentSize)                                                                    \
    {                                                                                                             \
        deviceGC();                                                                                               \
        A_size = experimentSize;                                                                                  \
        A_host = af::pinned<ctype>(size_t(experimentSize));                                                       \
    }                                                                                                             \
                                                                                                                  \
    virtual void tearDown()                                                                                       \
    {                                                                                                             \
        af::freePinned(A_host);                                                                                   \
    }                                                                                                             \
};                                                                                                                \
BENCHMARK_F( PinnedMemory, PinnedMemory_##dataType##_Bandwidth, PinnedFixture_2D_##dataType, samples, iterations) \
{                                                                                                                 \
    array B = array(A_size, A_host, afHost);                                                                      \
    B.eval();                                                                                                     \
}                                                                                                                 \


PinnedMemory_BENCHMARK(uint8_t, u8)
PinnedMemory_BENCHMARK(int16_t, s16)
PinnedMemory_BENCHMARK(int32_t, s32)
PinnedMemory_BENCHMARK(long long, s64)
PinnedMemory_BENCHMARK(float, f32)
PinnedMemory_BENCHMARK(double, f64)
