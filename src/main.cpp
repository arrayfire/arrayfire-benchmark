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
#include <string>

#include "arrayfire.h"
#include "af/version.h"
#include "AFResultTable.h"

using namespace celero;
using namespace std;

unsigned int samples = 10;
unsigned int operations = 10;

string image_directory = "";

// Wraps af::deviceprop with C++ data types. Handles allocation/deallocation of char*
bool getAFDeviceInfo(string & device_name, string & device_platform, string & device_toolkit, string & device_compute)
{
	char t_device_name[64] = {0};
	char t_device_platform[64] = {0};
	char t_device_toolkit[64] = {0};
	char t_device_compute[64] = {0};
	af::deviceInfo(t_device_name, t_device_platform, t_device_toolkit, t_device_compute);

	device_name = string(t_device_name);
	device_platform = string(t_device_platform);
	device_toolkit = string(t_device_toolkit);
	device_compute = string(t_device_compute);

	return af::isDoubleAvailable(af::getDevice());
}

/// Returns this computer's hostname
string getHostName()
{
#ifndef WIN32
    char hostname[1024] = "";
    gethostname(hostname, sizeof(hostname));
#else
    TCHAR hostname[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(hostname) / sizeof(hostname[0]);
    GetComputerName(hostname, &size);
#endif
    return string(hostname);
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
    string hostname = getHostName();


	bool enableImageBenchmarks = true;
#ifdef BENCHMARK_DATA_DIRECTORY
    image_directory = string(BENCHMARK_DATA_DIRECTORY);
#else
	enableImageBenchmarks = false;
#endif

	cmdline::parser args;
	args.add("list-benchmarks", '\0', "Prints a list of all available benchmarks.");
	args.add<string>("benchmark", 'b', "Runs a specific benchmark.", false, "");
	args.add<string>("exclude-benchmark", 'c', "Excludes a specific benchmark.", false, "");
	args.add<string>("group", 'g', "Runs a specific group of benchmarks.", false, "");
	args.add<string>("exclude-group", 'e', "Excludes a specific group of benchmarks.", false, "");
	args.add("list-devices", '\0', "Prints a list of all available devices on this backend.");
	args.add<uint64_t>("device", 'd',
        "Sets the device on which the benchmark will be executed", false);
	args.add<string>("replace-device-name", '\0',
        "Replace the device name returned by ArrayFire with this value.", false, "");
	args.add("disable-double", 's', "Manually disable all f64 tests.");
	args.parse_check(argc, argv);


	if(args.exist("list-benchmarks"))
	{
		int fieldWidth = 21;
		cout << endl;
		cout << "To run a specific group of benchmarks, run with -g group_name." << endl;
		cout << "To run a specific experiment, run with -b benchmark_name." << endl;
		cout << endl;
		cout << "List of benchmarks:" << endl;
		cout << " " << setw(fieldWidth + 1) << left << "Group" << "Benchmark" << endl;
		cout << " " << setw(fieldWidth + 1) << left << "---------" << "----------" << endl;
		vector<pair<string,string>> experiment_names = getExperimentNames();

		sort(experiment_names.begin(), experiment_names.end());
		for(auto experment_name: experiment_names)
			cout << " " << setw(fieldWidth) << left << experment_name.first << " " << experment_name.second << endl;

		return 0;
	}

	if(args.exist("list-devices"))
	{
		cout << "Available devices:" << endl;

		int nDevices = af::getDeviceCount();
		cout << " ID    Device               Platform   Toolkit              Compute" << endl;
		for(int device = 0; device < nDevices; device++)
		{
			af::setDevice(device);
			const bool doubleAvailable = getAFDeviceInfo(device_name, device_platform, device_toolkit, device_compute);

			device_name.resize(20, ' ');
			device_platform.resize(10, ' ');
			device_toolkit.resize(20, ' ');
			device_compute.resize(20, ' ');

			cout << " " << left << setw(5) << device << " " << device_name << " " << device_platform << " " << device_toolkit << " " << device_compute
			     << (doubleAvailable ? "" : "(Double precision not supported)")
			     << endl;
		}

		return 0;
	}

	auto intBackend = args.get<uint64_t>("device");
	if(intBackend > 0)
	{
		af::setDevice(intBackend);
	}

	// Initial output
	cout << fixed;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Green_Bold);
	cout << "[  CELERO  ]" << endl;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);

	celero::DisableDynamicCPUScaling();
    celero::timer::CachePerformanceFrequency();

    // Get information about ArrayFire
    string af_version(AF_VERSION);
    string af_revision(AF_REVISION);

    // Get information about the device on which we are running
    const bool doubleAvailable = getAFDeviceInfo(device_name, device_platform, device_toolkit, device_compute);
    // Permit the user to override poorly named devices
    if(args.exist("replace-device-name"))
        device_name = args.get<string>("replace-device-name");

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
    struct tm * timeinfo = gmtime(&current_time);
    string local_time = asctime(localtime(&current_time));
    local_time.erase(remove(local_time.begin(), local_time.end(), '\n'), local_time.end());
    char timestamp[1024];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H_%M_%S", timeinfo);

    stringstream filename;
    filename << timestamp << "_" << hostname << "_" << device_platform << "_"
        << device_name << ".csv";

    AFResultsTable::Instance().setFileName(filename.str());
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

	// get a list of all experiments
	vector<pair<string,string>> all_benchmarks = getExperimentNames();
	// select which tests we will execute
	vector<pair<string,string>> benchmarks;

	// find a specific experiment
	if(args.exist("benchmark"))
	{
		string benchmarkName = args.get<string>("benchmark");

		auto it = std::find_if(all_benchmarks.begin(), all_benchmarks.end(),
				[&benchmarkName](const pair<string, string> & p) { return p.second == benchmarkName; });

		// If the experiment was not found, exit the program
		if(it == all_benchmarks.end())
		{
			cout << "The benchmark named '" << benchmarkName << "' could not be found.";
			return 0;
		}

		// append the benchmark name to the list
		benchmarks.push_back((*it));
	}

	// exclude specific benchmark
	if(args.exist("exclude-benchmark"))
	{
		if(benchmarks.size() == 0)
			benchmarks = all_benchmarks;

		string benchmarkName = args.get<string>("exclude-benchmark");

		while(1) {
		    auto it = std::find_if(benchmarks.begin(), benchmarks.end(),
				[&benchmarkName](const pair<string, string> & p) { return p.second == benchmarkName; });
			if(it == benchmarks.end()) break;
			benchmarks.erase(it);
		}
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

	// exclude a sepecific group
	if(args.exist("exclude-group"))
	{
		if(benchmarks.size() == 0)
			benchmarks = all_benchmarks;

		string groupName = args.get<string>("exclude-group");

		for(int i = 0; i < int(benchmarks.size()); i++)
		{
			auto benchmark = benchmarks[i];
			if(benchmark.first == groupName)
			{
				auto loctemp = benchmarks.begin() + i;
				benchmarks.erase(loctemp);
				i--;
			}
		}
	}

	// If no test or group was specified, run all benchmarks.
	if(benchmarks.size() == 0)
		benchmarks = all_benchmarks;

	// Disable image benchmarks if the image directory was not specified.
	if(!enableImageBenchmarks)
	{
        cout << "No image directory was specified, disabling image benchmarks." << endl;

		for(int i = benchmarks.size() - 1; i > -1; i--)
		{
			if(benchmarks[i].first == "Image")
				benchmarks.erase(benchmarks.begin() + i);
		}
	}

    // Disable double tests based on user or device condition
	if(args.exist("disable-double") || !doubleAvailable)
	{
		for(int i = benchmarks.size() - 1; i > -1; i--)
		{
			auto benchmark = benchmarks[i];
			if( benchmark.first .find("f64") != std::string::npos ||
				benchmark.second.find("f64") != std::string::npos)
			{
				benchmarks.erase(benchmarks.begin() + i);
			}
		}
	}

    print::TableBanner();

	// run the tests
	for(auto benchmark: benchmarks)
	{
		string group = benchmark.first;
		string experiment = benchmark.second;
		executor::Run(group, experiment);
	}

	return 0;
}

