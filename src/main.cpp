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
#include <iomanip>
#include <fstream>
#include <cmath>
#include <ctime>

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
	char t_device_name[64] = {0};
	char t_device_platform[64] = {0};
	char t_device_toolkit[64] = {0};
	char t_device_compute[64] = {0};
	af::deviceprop(t_device_name, t_device_platform, t_device_toolkit, t_device_compute);

	device_name = string(t_device_name);
	device_platform = string(t_device_platform);
	device_toolkit = string(t_device_toolkit);
	device_compute = string(t_device_compute);
}

int main(int argc, char** argv)
{
	string device_name, device_platform, device_toolkit, device_compute;

	cmdline::parser args;
	args.add("list", 'l', "Prints a list of all available benchmarks and devices.");
	args.add<uint64_t>("devices", 'd', "Sets the backend on which the benchmark will be executed", false);
	args.add<std::string>("group", 'g', "Runs a specific group of benchmarks.", false, "");
	args.add<std::string>("recordTable", 'r', "Appends the results table to the named file.", false, "");
//	args.add<std::string>("outputTable", 't', "Saves a results table to the named file.", false, "");
//	args.add<std::string>("junit", 'j', "Saves a JUnit XML-formatted file to the named file.", false, "");
//	args.add<std::string>("archive", 'a', "Saves or updates a result archive file.", false, "");
//	args.add<uint64_t>("distribution", 'd', "Builds a file to help characterize the distribution of measurements and exits.", false, 0);
	args.parse_check(argc, argv);

	if(args.exist("list"))
	{
		cout << "Available devices:" << endl;

		int nDevices = af::getDeviceCount();
		cout << "ID    Device               Platform   Toolkit              Compute" << endl;
		for(int device = 0; device < nDevices; device++)
		{
			af::setDevice(device);
			getAFDeviceInfo(device_name, device_platform, device_toolkit, device_compute);

			device_name.resize(20, ' ');
			device_platform.resize(10, ' ');
			device_toolkit.resize(20, ' ');
			device_compute.resize(20, ' ');

			cout << left << setw(5) << device << " " << device_name << " " << device_platform << " " << device_toolkit << " " << device_compute << endl;
		}

		cout << endl;
		cout << "Available tests:" << endl;
		TestVector& tests = celero::TestVector::Instance();
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

	auto intDevice = args.get<uint64_t>("devices");
	if(intDevice > 0)
	{
		af::setDevice(intDevice);
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
		getAFDeviceInfo(device_name, device_platform, device_toolkit, device_compute);

		// Get the current time. Strip the newline from asctime(...)
		std::time_t current_time = std::time(nullptr);
		string local_time = std::asctime(std::localtime(&current_time));
		local_time.erase(std::remove(local_time.begin(), local_time.end(), '\n'), local_time.end());

		AFResultsTable::Instance().setFileName(argument);
		AFResultsTable::Instance().addStaticColumn("AF_VERSION", af_version);
		AFResultsTable::Instance().addStaticColumn("AF_REVISION", af_revision);
		AFResultsTable::Instance().addStaticColumn("AF_DEVICE", device_name);
		AFResultsTable::Instance().addStaticColumn("AF_PLATFORM", device_platform);
		AFResultsTable::Instance().addStaticColumn("AF_TOOLKIT", device_toolkit);
		AFResultsTable::Instance().addStaticColumn("AF_COMPUTE", device_compute);
		AFResultsTable::Instance().addStaticColumn("LOCAL_TIME", local_time);
		AFResultsTable::Instance().addStaticColumn("POSIX_TIME", std::to_string(current_time));


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

