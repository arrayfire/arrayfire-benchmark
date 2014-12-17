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

// Benchmarks for image tests
BASELINE_F(Image, Baseline, Fixture_Image_Directory, samples, operations)
{

}

BENCHMARK_F(Image, Open, Fixture_Image_Directory, 1, 3)
{
	for(auto filename: this->filenames)
	{
		try
		{
			array A = af::loadimage(filename.c_str(), true);
		}
		catch (af::exception & e)
		{
			// do nothing
		}
	}
}

BENCHMARK_F(Image, Rotate90, Fixture_Image_Directory, 1, 3)
{
	for(auto filename: this->filenames)
	{
		try
		{
			array A = af::loadimage(filename.c_str(), true);
			array B = af::rotate(A, 90);
			B.eval();
		}
		catch (af::exception & e)
		{
			// do nothing
		}
	}

	af::sync();
}
