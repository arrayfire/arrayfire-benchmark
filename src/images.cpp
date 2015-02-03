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
#include <glob.h>

using namespace af;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

unsigned int image_samples = 1;
unsigned int image_operations = 3;
extern std::string image_directory;

array load_image(std::string filename);

// Run a glob on a specific directory.
///
/// This function uses unix/linux specific functionality.
std::vector<std::string> glob(const std::string& pat)
{
    using namespace std;
    glob_t glob_result;
    glob(pat.c_str(),GLOB_TILDE,NULL,&glob_result);
    vector<string> ret;
    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        ret.push_back(string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return ret;
}

// Base class for directories of images
class Fixture_Image_Directory : public celero::TestFixture
{
public:
    std::vector<std::string> filenames;
    std::vector<af::array> images;
    unsigned int max_images = 1000;

    Fixture_Image_Directory(){}

    virtual std::vector<int64_t> getExperimentValues() const
    {
        std::vector<int64_t> sizes;
        sizes.push_back(max_images);
        return sizes;
    }

    /// Before each run, build a vector of random integers.
    virtual void setUp(int64_t experimentSize)
    {
        if(image_directory.size() > 0)
        {
            fs::path directory(image_directory);
            fs::path directory_full = fs::complete(directory);
            filenames = glob(directory_full.string() + "/*.jpg");
            
            // preload the images to the GPU
            int max_images = 1000;
            for(auto filename: filenames)
            {
                if(images.size() > max_images)
                    return;

                array A = load_image(filename);
                if(!A.isempty())
                    images.push_back(A);
            }
        }
    }
};

array load_image(std::string filename)
{
    array temp;

    try
    {
        temp = af::loadimage(filename.c_str(), false);
    }
    catch (af::exception & e)
    {
        // do nothing
    }

    return temp;
}



// Benchmarks for image tests
BASELINE_F(Image, Baseline, Fixture_Image_Directory,  image_samples, image_operations)
{
}

BENCHMARK_F(Image, Histogram, Fixture_Image_Directory,  image_samples, image_operations)
{
    for(auto image: this->images)
    {
        array B = histogram(image, 256, 0, 255);
    }

    af::sync();
}

BENCHMARK_F(Image, Resize_Shrink_2x, Fixture_Image_Directory,  image_samples, image_operations)
{
    for(auto image: this->images)
    {
        array B = af::resize(0.5, image, AF_INTERP_NEAREST);
    }

    af::sync();
}

BENCHMARK_F(Image, Resize_Expand_2x, Fixture_Image_Directory,  image_samples, image_operations)
{
    for(auto image: this->images)
    {
    
        array B = af::resize(2, image, AF_INTERP_NEAREST);
    }

    af::sync();
}

BENCHMARK_F(Image, Convolve_5x5, Fixture_Image_Directory,  image_samples, image_operations)
{
    array K = randu(5, 5, f32);
    for(auto image: this->images)
    {
        array B = af::convolve2(image, K, false);
    }

    af::sync();
}


BENCHMARK_F(Image, Convolve_9x9, Fixture_Image_Directory,  image_samples, image_operations)
{
    array K = randu(9, 9, f32);
    for(auto image: this->images)
    {
        array B = af::convolve2(image, K, false);
    }

    af::sync();
}


BENCHMARK_F(Image, Convolve_11x11, Fixture_Image_Directory,  image_samples, image_operations)
{
    array K = randu(11, 11, f32);
    for(auto image: this->images)
    {
        array B = af::convolve2(image, K, false);
    }

    af::sync();
}

BENCHMARK_F(Image, Erode_5x5, Fixture_Image_Directory,  image_samples, image_operations)
{
    array K = randu(5, 5, f32);
    for(auto image: this->images)
    {
        array B = af::erode(image, K);
    }

    af::sync();
}

BENCHMARK_F(Image, Bilateral, Fixture_Image_Directory,  image_samples, image_operations)
{
    array K = randu(5, 5, f32);
    for(auto image: this->images)
    {
        array B = bilateral(image, 2.5f, 50.0f);
    }

    af::sync();
}

BENCHMARK_F(Image, FAST, Fixture_Image_Directory,  image_samples, image_operations)
{
    for(auto image: this->images)
    {
        af::features features = af::fast(image, 20, 9, 0, 0.05f);
    }

    af::sync();
}



