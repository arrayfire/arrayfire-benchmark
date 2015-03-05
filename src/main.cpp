#include <celero/Celero.h>
#include <celero/Archive.h>
#include <celero/Celero.h>
#include <celero/CommandLine.h>
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
string image_directory = "";

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

/// Get a vector containing all pairs of (group, benchmark) registered with Celero
vector<pair<string,string>> getExperimentNames()
{
	vector<pair<string,string>> names;

	TestVector& tests = celero::TestVector::Instance();
	for(::size_t i = 0; i < tests.size(); i++)
	{
		auto bm = celero::TestVector::Instance()[i];
		string parent_name = bm->getName();
		for(::size_t j = 0; j < bm->getExperimentSize(); j++)
		{
			auto experiment = bm->getExperiment(j);
			string experiment_name = experiment->getName();
			auto temp = make_pair(parent_name, experiment_name);
			names.push_back(temp);
		}
	}

	return names;
}

int main(int argc, char** argv)
{
	string device_name, device_platform, device_toolkit, device_compute;

	bool disableImageBenchmarks = false;

	cmdline::parser args;
	args.add("list-benchmarks", '\0', "Prints a list of all available benchmarks.");
	args.add<string>("group", 'g', "Runs a specific group of benchmarks.", false, "");
	args.add<string>("experiment", 'e', "Runs a specific benchmark.", false, "");
	args.add("list-backends", '\0', "Prints a list of all available benchmarks.");
	args.add<uint64_t>("backend", 'b', "Sets the backend on which the benchmark will be executed", false);
	args.add<string>("recordTable", 'r', "Appends the results table to the named file.", false, "");
//	args.add<string>("outputTable", 't', "Saves a results table to the named file.", false, "");
//	args.add<string>("junit", 'j', "Saves a JUnit XML-formatted file to the named file.", false, "");
//	args.add<string>("archive", 'a', "Saves or updates a result archive file.", false, "");
//	args.add<uint64_t>("distribution", 'd', "Builds a file to help characterize the distribution of measurements and exits.", false, 0);
	args.footer("[image_directory]");
	args.parse_check(argc, argv);

	if(args.rest().size() > 0)
	{
		image_directory = args.rest()[0];
	}
    else
    {
        disableImageBenchmarks = true;
    }

	if(args.exist("list-benchmarks"))
	{
		int fieldWidth = 21;
		cout << endl;
		cout << "To run a specific group of benchmarks, run with -g group_name." << endl;
		cout << "To run a specific experiment, run with -e experiment_name." << endl;
		cout << endl;
		cout << "List of benchmarks:" << endl;
		cout << " " << setw(fieldWidth + 1) << left << "Benchmark" << "Experiment" << endl;
		cout << " " << setw(fieldWidth + 1) << left << "---------" << "----------" << endl;
		vector<pair<string,string>> experiment_names = getExperimentNames();

		sort(experiment_names.begin(), experiment_names.end());
		for(auto experment_name: experiment_names)
			cout << " " << setw(fieldWidth) << left << experment_name.first << " " << experment_name.second << endl;

		return 0;
	}

	if(args.exist("list-backends"))
	{
		cout << "Available devices:" << endl;

		int nDevices = af::getDeviceCount();
		cout << " ID    Device               Platform   Toolkit              Compute" << endl;
		for(int device = 0; device < nDevices; device++)
		{
			af::setDevice(device);
			getAFDeviceInfo(device_name, device_platform, device_toolkit, device_compute);

			device_name.resize(20, ' ');
			device_platform.resize(10, ' ');
			device_toolkit.resize(20, ' ');
			device_compute.resize(20, ' ');

			cout << " " << left << setw(5) << device << " " << device_name << " " << device_platform << " " << device_toolkit << " " << device_compute << endl;
		}

		return 0;
	}

	auto intBackend = args.get<uint64_t>("backend");
	if(intBackend > 0)
	{
		af::setDevice(intBackend);
	}

	// Initial output
	print::GreenBar("");
	cout << fixed;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Green_Bold);
	cout << "[  CELERO  ]" << endl;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);

	celero::print::GreenBar("");
	celero::timer::CachePerformanceFrequency();

	// Has a result output file been specified?
	auto argument = args.get<string>("recordTable");
	if(argument.empty() == false)
	{
		// Get information about ArrayFire
		string af_version = string(AF_VERSION) + string(AF_VERSION_MINOR);
		string af_revision(AF_REVISION);

		// Get information about the device on which we are running
		getAFDeviceInfo(device_name, device_platform, device_toolkit, device_compute);
		
		if(device_name.size() == 0)
		    device_name = "UNKNOWN";
		if(device_platform.size() == 0)
		    device_platform = "UNKNOWN";
		if(device_toolkit.size() == 0)
		    device_toolkit = "UNKNOWN";
		if(device_compute.size() == 0)
            device_compute = "UNKNOWN";
            
		// Get the current time. Strip the newline from asctime(...)
		time_t current_time = time(nullptr);
		string local_time = asctime(localtime(&current_time));
		local_time.erase(remove(local_time.begin(), local_time.end(), '\n'), local_time.end());

		AFResultsTable::Instance().setFileName(argument);
		AFResultsTable::Instance().addStaticColumn("AF_VERSION", af_version);
		AFResultsTable::Instance().addStaticColumn("AF_REVISION", af_revision);
		AFResultsTable::Instance().addStaticColumn("AF_DEVICE", device_name);
		AFResultsTable::Instance().addStaticColumn("AF_PLATFORM", device_platform);
		AFResultsTable::Instance().addStaticColumn("AF_TOOLKIT", device_toolkit);
		AFResultsTable::Instance().addStaticColumn("AF_COMPUTE", device_compute);
		AFResultsTable::Instance().addStaticColumn("LOCAL_TIME", local_time);
		AFResultsTable::Instance().addStaticColumn("POSIX_TIME", to_string(current_time));


		celero::AddExperimentResultCompleteFunction(
			[](shared_ptr<celero::Result> p)
			{
				AFResultsTable::Instance().add(p);
			});
	}

	// get a list of all experiments
	vector<pair<string,string>> all_benchmarks = getExperimentNames();
	// select which tests we will execute
	vector<pair<string,string>> benchmarks;

	// find a specific experiment
	if(args.exist("experiment"))
	{
		string experimentName = args.get<string>("experiment");

		auto it = std::find_if(all_benchmarks.begin(), all_benchmarks.end(),
				[&experimentName](const pair<string, string> & p) { return p.second == experimentName; });

		// If the experiment was not found, exit the program
		if(it == all_benchmarks.end())
		{
			cout << "The experiment named '" << experimentName << "' could not be found.";
			return 0;
		}

		// append the benchmark name to the list
		benchmarks.push_back((*it));
	}

	// find a sepecific group
	if(args.exist("group"))
	{
		bool foundGroup = false;

		string groupName = args.get<string>("group");

		for(int i = 0; i < int(all_benchmarks.size()); i++)
		{
			auto benchmark = all_benchmarks[i];
			if(benchmark.first == groupName)
			{
				benchmarks.push_back(benchmark);
				foundGroup = true;
			}

		}

		// If the experiment was not found, exit the program
		if(!foundGroup)
		{
			cout << "The group named '" << groupName << "' could not be found.";
			return 0;
		}
	}

	// If no test or group was specified, run all benchmarks.
	if(benchmarks.size() == 0)
		benchmarks = all_benchmarks;

	// Disable image benchmarks if the image directory was not specified.
	if(disableImageBenchmarks)
	{
        cout << "No image directory was specified, disabling image benchmarks." << endl;

		for(int i = benchmarks.size() - 1; i > -1; i--)
		{
			if(benchmarks[i].first == "Image")
				benchmarks.erase(benchmarks.begin() + i);
		}
	}

	// run the tests
	for(auto benchmark: benchmarks)
	{
		string group = benchmark.first;
		string experiment = benchmark.second;
		executor::Run(group, experiment);
	}

	// Final output.
	string finalOutput;
	print::StageBanner(finalOutput);

	return 0;
}

