#include <celero/Celero.h>
#include <celero/Archive.h>
#include <celero/Celero.h>
#include <celero/Console.h>
#include <celero/Benchmark.h>
#include <celero/TestVector.h>
#include <celero/Utilities.h>
#include <celero/Executor.h>
#include <celero/Print.h>
#include <celero/ResultTable.h>
#include <celero/JUnit.h>
#include <celero/CommandLine.h>
#include <celero/Distribution.h>
#include <celero/Callbacks.h>

#include <iostream>
#include <fstream>
#include <cmath>

#include "arrayfire.h"
#include "af/version.h"
#include "AFResultTable.h"

using namespace celero;
using namespace std;

unsigned int samples = 10;
unsigned int operations = 10;

// Wraps af::deviceprop with C++ data types. Handles allocation/deallocation of char*
void getAFDeviceInfo(string & device_name, string & device_platform, string & device_toolkit, string & device_compute)
{
	// allocate 32 characters (32 bytes + 1)
	size_t  temp_alloc = 33 * sizeof(char);
	char * t_device_name = (char*) malloc(temp_alloc);
	char * t_device_platform = (char*) malloc(temp_alloc);
	char * t_device_toolkit = (char*) malloc(temp_alloc);
	char * t_device_compute = (char*) malloc(temp_alloc);
	af::deviceprop(t_device_name, t_device_platform, t_device_toolkit, t_device_compute);

	device_name = string(t_device_name);
	device_platform = string(t_device_platform);
	device_toolkit = string(t_device_toolkit);
	device_compute = string(t_device_compute);

	delete t_device_name;
	delete t_device_platform;
	delete t_device_toolkit;
	delete t_device_compute;

}

int main(int argc, char** argv)
{
	cmdline::parser args;
	args.add("list", 'l', "Prints a list of all available benchmarks.");
	args.add<std::string>("group", 'g', "Runs a specific group of benchmarks.", false, "");
	args.add<std::string>("recordTable", 'r', "Appends the results table to the named file.", false, "");
//	args.add<std::string>("outputTable", 't', "Saves a results table to the named file.", false, "");
//	args.add<std::string>("junit", 'j', "Saves a JUnit XML-formatted file to the named file.", false, "");
//	args.add<std::string>("archive", 'a', "Saves or updates a result archive file.", false, "");
//	args.add<uint64_t>("distribution", 'd', "Builds a file to help characterize the distribution of measurements and exits.", false, 0);
	args.parse_check(argc, argv);

	if(args.exist("list"))
	{
		TestVector& tests = celero::TestVector::Instance();
		std::cout << "Avaliable tests:" << std::endl;
		std::vector<std::string> test_names;
		for(unsigned int i = 0; i < tests.size(); i++)
		{
			auto bm = celero::TestVector::Instance()[i];
			test_names.push_back(bm->getName());
		}

		std::sort(test_names.begin(), test_names.end());
		for(auto test_name: test_names)
			std::cout << " " << test_name << std::endl;

		return 0;
	}


	// Initial output
	print::GreenBar("");
	std::cout << std::fixed;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Green_Bold);
	std::cout << "[  CELERO  ]" << std::endl;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);

	celero::print::GreenBar("");
	celero::timer::CachePerformanceFrequency();

	// Has a result output file been specified?
	auto argument = args.get<std::string>("recordTable");
	if(argument.empty() == false)
	{
		// Get information about ArrayFire
		string af_version = string(AF_VERSION) + string(AF_VERSION_MINOR);
		string af_revision(AF_REVISION);

		// Get information about the device on which we are running
		string device_name, device_platform, device_toolkit, device_compute;
		getAFDeviceInfo(device_name, device_platform, device_toolkit, device_compute);

		AFResultsTable::Instance().setFileName(argument);
		AFResultsTable::Instance().addStaticColumn("AF_VERSION", af_version);
		AFResultsTable::Instance().addStaticColumn("AF_REVISION", af_revision);
		AFResultsTable::Instance().addStaticColumn("AF_DEVICE", device_name);
		AFResultsTable::Instance().addStaticColumn("AF_PLATFORM", device_platform);
		AFResultsTable::Instance().addStaticColumn("AF_TOOLKIT", device_toolkit);
		AFResultsTable::Instance().addStaticColumn("AF_COMPUTE", device_compute);


		celero::AddExperimentResultCompleteFunction(
			[](std::shared_ptr<celero::Result> p)
			{
				AFResultsTable::Instance().add(p);
			});
	}

	std::string finalOutput;

	// Has a run group been specified?
	argument = args.get<std::string>("group");
	if(argument.empty() == false)
	{
		executor::Run(argument);
	}
	else
	{
		executor::RunAll();
	}

	// Final output.
	print::StageBanner(finalOutput);

	return 0;
}

