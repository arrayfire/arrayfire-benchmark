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

extern unsigned int samples;
extern unsigned int operations;
extern std::string image_directory;

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

	Fixture_Image_Directory(){}

	virtual std::vector<int64_t> getExperimentValues() const
	{
		std::vector<int64_t> sizes;
		unsigned int max_images = 0;

		if(image_directory.size() > 0)
		{
			fs::path directory(image_directory);
			fs::path directory_full = fs::complete(directory);
			std::vector<std::string> temp = glob(directory_full.string() + "/*.jpg");
			max_images = temp.size();
		}

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
BASELINE_F(Image, Baseline, Fixture_Image_Directory, 1, 1)
{
	for(auto filename: this->filenames)
	{
		array A = load_image(filename);
	}
}

BENCHMARK_F(Image, Histogram, Fixture_Image_Directory, 1, 1)
{
	for(auto filename: this->filenames)
	{
		array A = load_image(filename);
		if(!A.isempty())
		{
			array B = histogram(A, 256, 0, 255);
		}
	}

	af::sync();
}

BENCHMARK_F(Image, Resize_Shrink_2x, Fixture_Image_Directory, 1, 1)
{
	for(auto filename: this->filenames)
	{
		array A = load_image(filename);
		if(!A.isempty())
		{
			array B = af::resize(0.5, A, AF_INTERP_NEAREST);
		}
	}

	af::sync();
}

BENCHMARK_F(Image, Resize_Expand_2x, Fixture_Image_Directory, 1, 1)
{
	for(auto filename: this->filenames)
	{
		array A = load_image(filename);
		if(!A.isempty())
		{
			array B = af::resize(2, A, AF_INTERP_NEAREST);
		}
	}

	af::sync();
}

BENCHMARK_F(Image, Convolve_5x5, Fixture_Image_Directory, 1, 1)
{
	array K = randu(5, 5, f32);
	for(auto filename: this->filenames)
	{
		array A = load_image(filename);
		if(!A.isempty())
		{
			array B = af::convolve2(A, K, false);
		}
	}

	af::sync();
}


BENCHMARK_F(Image, Convolve_9x9, Fixture_Image_Directory, 1, 1)
{
	array K = randu(9, 9, f32);
	for(auto filename: this->filenames)
	{
		array A = load_image(filename);
		if(!A.isempty())
		{
			array B = af::convolve2(A, K, false);
		}
	}

	af::sync();
}


BENCHMARK_F(Image, Convolve_11x11, Fixture_Image_Directory, 1, 1)
{
	array K = randu(11, 11, f32);
	for(auto filename: this->filenames)
	{
		array A = load_image(filename);
		if(!A.isempty())
		{
			array B = af::convolve2(A, K, false);
		}
	}

	af::sync();
}

BENCHMARK_F(Image, Erode_5x5, Fixture_Image_Directory, 1, 1)
{
	array K = randu(5, 5, f32);
	for(auto filename: this->filenames)
	{
		array A = load_image(filename);
		if(!A.isempty())
		{
			array B = af::erode(A, K);
		}
	}

	af::sync();
}

BENCHMARK_F(Image, Bilateral, Fixture_Image_Directory, 1, 1)
{
	array K = randu(5, 5, f32);
	for(auto filename: this->filenames)
	{
		array A = load_image(filename);
		if(!A.isempty())
		{
			array B = bilateral(A, 2.5f, 50.0f);
		}
	}

	af::sync();
}

BENCHMARK_F(Image, FAST, Fixture_Image_Directory, 1, 1)
{
	for(auto filename: this->filenames)
	{
		array A = load_image(filename);	// load in grayscale
		if(!A.isempty())
		{
			af::features features = af::fast(A, 20, 9, 0, 0.05f);
		}
	}

	af::sync();
}

BENCHMARK_F(Image, ORB, Fixture_Image_Directory, 1, 1)
{
	for(auto filename: this->filenames)
	{
		array A = load_image(filename);	// load must be in grayscale
		if(!(A.isempty()))
		{
			af::features features;
			af::array desc;
			// use same parameters as OpenCV for fair benchmark, see
			// https://github.com/Itseez/opencv/blob/master/modules/features2d/include/opencv2/features2d.hpp
			af::orb(features, desc, A, 20.0f, 500, 1.2f, 8);
		}
	}
}

//BENCHMARK_F(Image, ColorConverstion_RGB_to_Gray, Fixture_Image_Directory, 1, 1)
//{
//#warning "Colorspace is not implemented in this version of ArrayFire"
//	for(auto filename: this->filenames)
//	{
//		array A;
//
//		try
//		{
//			A = af::loadimage(filename.c_str(), false);
//		}
//		catch (af::exception & e)
//		{
//			// do nothing
//		}
//
//		if(!A.isempty())
//		{
//			B = colorspace	(A, af_rgb, af_gray);
//		}
//	}
//
//	af::sync();
//}

