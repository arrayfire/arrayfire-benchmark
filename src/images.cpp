/*
 * images.cpp
 *
 *  Created on: Dec 17, 2014
 *      Author: bkloppenborg
 */

#include <math.h>
#include <celero/Celero.h>
#include <arrayfire.h>
#include "fixtures.h" // defines most test figures
#include <sstream>

using namespace af;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

unsigned int image_samples = 10;
unsigned int image_operations = 10;
extern std::string image_directory;

// Base class for directories of images
class FixtureImage : public AF_Fixture
{
public:
    af::array image;

    FixtureImage(){}

    virtual std::vector<std::pair<int64_t, uint64_t>> getExperimentValues() const
    {
        std::vector<std::pair<int64_t, uint64_t>> sizes;
        if(image_directory.size() > 0)
        {
            sizes.push_back(std::make_pair<int64_t, uint64_t>(240, 0));
            sizes.push_back(std::make_pair<int64_t, uint64_t>(480, 0));
            sizes.push_back(std::make_pair<int64_t, uint64_t>(720, 0));
            sizes.push_back(std::make_pair<int64_t, uint64_t>(1080, 0));
            sizes.push_back(std::make_pair<int64_t, uint64_t>(3840, 0));
        }
        if(this->use_max_problemspace && sizes.size() > 1){
            sizes.erase(sizes.begin(), sizes.end() - 1);
        }
        return sizes;
    }

    /// Before each run, build a vector of random integers.
    virtual void setUp(int64_t experimentSize)
    {
        if(experimentSize > 0)
        {
            std::stringstream filename;
            filename << image_directory << "/" << experimentSize << "p-img-0009999.jpeg";
            try {
                deviceGC();
                image = af::loadImage(filename.str().c_str(), true);
                image = af::rgb2gray(image, 0.2126, 0.7152, 0.0722);
                image.eval();
            }
            catch (af::exception & e)
            {
                // rethrow the exception
                throw e;
            }
        }
    }
};

class FixtureImageWithKernel : public FixtureImage
{
public:
    af::array K_5x5;
    af::array K_9x9;
    af::array K_11x11;

    FixtureImageWithKernel() {}

    virtual void setUp(int64_t experimentSize)
    {
        // use the standard image setup function and create a few defined
        // kernel sizes
        FixtureImage::setUp(experimentSize);
        K_5x5   = randu(5, 5, f32);
        K_5x5.eval();
        K_9x9   = randu(9, 9, f32);
        K_9x9.eval();
        K_11x11 = randu(11, 11, f32);
        K_11x11.eval();
        af::sync();
    }
};

class FixtureGrayscaleImage : public FixtureImage
{
public:

    FixtureGrayscaleImage() {}

    virtual void setup(int64_t experimentSize)
    {
        // use the standard image setup function, but convert the image to
        // grayscale
        FixtureImage::setUp(experimentSize);
        image = af::rgb2gray(image, 0.2126, 0.7152, 0.0722);
        image.eval();
    }

};

// Benchmarks for image tests
BASELINE_F(Image, Baseline, FixtureImage,  image_samples, image_operations) {}

// Macro to simplify the creation of benchmarks.
// Here benchmarkFunction will be an ArrayFire function name. The variadic
// portion corresponds to the arguments passed to the function. To specify the
// image, use the variable "image" (from the FixtureImage class)
#define IMAGE_BENCHMARK(benchmarkName, benchmarkFunction, ...)    \
BENCHMARK_F(Image, Image_##benchmarkName , FixtureImage, \
    image_samples, image_operations)                \
{                                                   \
    array B = benchmarkFunction ( __VA_ARGS__ );    \
}                                                   \

//              Benchmark Name      Function        Arguments
IMAGE_BENCHMARK(Histogram,          af::histogram,  image, 256, 0, 255)
IMAGE_BENCHMARK(Resize_Shrink_2x,   af::resize,     0.5, image, AF_INTERP_NEAREST)
IMAGE_BENCHMARK(Resize_Expand_2x,   af::resize,     2.0, image, AF_INTERP_NEAREST)

// Macro to simplify the creation of benchmarks.
// Here benchmarkFunction will be an ArrayFire function name. The variadic
// portion corresponds to the arguments passed to the function. To specify the
// image, use the variable "image" (from the FixtureImage class)
#define IMAGE_KERNEL_BENCHMARK(benchmarkName, benchmarkFunction, ...)    \
BENCHMARK_F(Image, Image_##benchmarkName , FixtureImageWithKernel, \
    image_samples, image_operations)                \
{                                                   \
    array B = benchmarkFunction ( __VA_ARGS__ );    \
}

//                     Benchmark Name    Function        Arguments
IMAGE_KERNEL_BENCHMARK(Convolve_5x5,     af::convolve2,  image, K_5x5)
IMAGE_KERNEL_BENCHMARK(Convolve_9x9,     af::convolve2,  image, K_9x9)
IMAGE_KERNEL_BENCHMARK(Convolve_11x11,   af::convolve2,  image, K_11x11)

IMAGE_KERNEL_BENCHMARK(Erode_5x5,        af::erode,      image, K_5x5)
IMAGE_KERNEL_BENCHMARK(Erode_9x9,        af::erode,      image, K_9x9)
IMAGE_KERNEL_BENCHMARK(Erode_11x11,      af::erode,      image, K_11x11)

IMAGE_KERNEL_BENCHMARK(Bilateral_5x5,    af::bilateral,  image, 2.5f, 50.0f)
IMAGE_KERNEL_BENCHMARK(Bilateral_9x9,    af::bilateral,  image, 2.5f, 50.0f)
IMAGE_KERNEL_BENCHMARK(Bilateral_11x11,  af::bilateral,  image, 2.5f, 50.0f)

// Other remaining benchmarks
BENCHMARK_F(Image, Image_FAST, FixtureImage,  image_samples, image_operations)
{
    af::features features = af::fast(image, 20, 9, 0, 0.05f);
}

BENCHMARK_F(Image, Image_ORB, FixtureImage,  image_samples, image_operations)
{
    af::features features;
    af::array descriptions;
    // At present af::orb will reduce the image by half N-times. When fewer than
    // 8 pixels remain, it will thrown an error. Here we automatically determine
    // the maximum level which ORB can process safely:
    int min_size = std::min(image.dims()[0], image.dims()[1]);
    int max_level = log2(min_size / 8);
    af::orb(features, descriptions, image, 20, 500, 1.2, max_level);
}
