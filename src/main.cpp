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
#include "fixtures.h"

#include "boost/program_options.hpp"
#include "boost/algorithm/string.hpp"

using namespace celero;
using namespace std;
namespace po = boost::program_options;


unsigned int samples = 10;
unsigned int operations = 10;
std::vector<std::pair<int64_t, uint64_t>> AF_Fixture::overrideProblemSpace;

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

template<typename T>
struct csv_option
{
    std::vector<T> values;
};

template<typename T>
void validate(boost::any& v, const std::vector<std::string>& values, csv_option<T>* target_type, int)
{
    namespace alg = boost::algorithm;
    std::vector<T> result;
    typedef std::vector<std::string> strings;
    for(strings::const_iterator iter = values.begin(); iter != values.end(); ++iter )
    {
        strings tks;
        alg::split(tks, *iter, alg::is_any_of(","));
        for(strings::const_iterator tk = tks.begin(); tk != tks.end(); ++tk)
        {
            result.push_back(boost::lexical_cast<T>(*tk));
        }
    }
    v = csv_option<T>();
    boost::any_cast<csv_option<T>&>(v).values.swap(result);
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

    try {
        po::options_description desc("Options");
        desc.add_options()
            ("help,h"                   , "Print help messages")
            ("list-benchmarks,l"        , "Prints a list of all available benchmarks.")
            ("benchmark,b"              , po::value<vector<string>>(), "Runs a specific benchmark.")
            ("problem-space"            , po::value<csv_option<uint64_t>>()->multitoken(), "override test problemspace defaults")
            ("exclude-benchmark,e"      , po::value<vector<string>>(), "Excludes a specific benchmark.")
            ("group,g"                  , po::value<vector<string>>(), "Runs a specific group of benchmarks.")
            ("exclude-group,x"          , po::value<vector<string>>(), "Excludes a specific group of benchmarks.")
            ("start-from-group,c"       , po::value<vector<string>>(), "Run benchmarks starting from specified group.")
            ("list-devices,i"           , "Prints a list of all available devices on this backend.")
            ("device,d"                 , po::value<uint64_t>()->default_value(0), "Sets the device on which the benchmark will be executed")
            ("replace-device-name,r"    , po::value<string>(), "Replace the device name returned by ArrayFire with this value.")
            ("disable-double,s"         , "Manually disable all f64 tests.")
            ("print,p"                  , "Print list of tests selected before running")
        ;

        po::variables_map vm;
        try {
            po::store(po::parse_command_line(argc, argv, desc),
                      vm);

            ////////////////////////////////////////////////////////////////////////
            // Help
            ////////////////////////////////////////////////////////////////////////
            if (vm.count("help")) {
                cout << "ArrayFire Benchmarking Options" << endl << desc << endl;
                return 0;
            }
            po::notify(vm);
        } catch(po::error& e) {
            cout << endl << e.what() << desc << endl;
            return 2;
        }

        ////////////////////////////////////////////////////////////////////////////
        // List all benchmarks
        ////////////////////////////////////////////////////////////////////////////
        if(vm.count("list-benchmarks"))
        {
            int fieldWidth = 24;
            cout << endl;
            cout << "To run a specific group of benchmarks, run with -g group_name." << endl;
            cout << "To run a specific experiment, run with -b benchmark_name." << endl;
            cout << "To exclude a specific group of benchmarks, run with -x group_name." << endl;
            cout << "To start from a specific group, run with -c group_name." << endl;
            cout << "To exclude a specific experiment, run with -e benchmark_name." << endl;
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

        ////////////////////////////////////////////////////////////////////////////
        // List available devices
        ////////////////////////////////////////////////////////////////////////////
        if(vm.count("list-devices"))
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

        ////////////////////////////////////////////////////////////////////////////
        // Set device - Will always enter since default options is specified
        ////////////////////////////////////////////////////////////////////////////
        if(vm.count("device"))
        {
            uint64_t deviceID = vm["device"].as<uint64_t>();
            af::setDevice(deviceID);
        }

        ////////////////////////////////////////////////////////////////////////////
        // Print table headers & initalize celero
        ////////////////////////////////////////////////////////////////////////////
        cout << fixed;
        celero::console::SetConsoleColor(celero::console::ConsoleColor_Green_Bold);
        cout << "[  CELERO  ]" << endl;
        celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);

        celero::DisableDynamicCPUScaling();
        celero::timer::CachePerformanceFrequency();

        ////////////////////////////////////////////////////////////////////////////
        // Get information about ArrayFire
        ////////////////////////////////////////////////////////////////////////////
        string af_version(AF_VERSION);
#if AF_VERSION_MAJOR >= 3 && AF_VERSION_MINOR >= 3
        string af_revision(af_get_revision());
#elif defined(AF_REVISION)
        string af_revision(AF_REVISION);
#else
        string af_revision("default");
#endif

        ////////////////////////////////////////////////////////////////////////////
        // Get information about the device on which we are running
        ////////////////////////////////////////////////////////////////////////////
        const bool doubleAvailable = getAFDeviceInfo(device_name, device_platform, device_toolkit, device_compute);

    // Roughly detect the host operating system.
#ifdef _WIN32
    string host_operating_system("Windows");
#elif __APPLE__
    string host_operating_system("OSX");
#elif __linux__
    string host_operating_system("Linux");
#elif __unix__
    string host_operating_system("Unix");
#elif defined(_POSIX_VERSION)
    string host_operating_system("POSIX");
#else
    string host_operating_system("Unknown");
#endif

        ////////////////////////////////////////////////////////////////////////////
        // User assigned name replacement
        ////////////////////////////////////////////////////////////////////////////
        if(vm.count("replace-device-name")) {
            string device_name_t = vm["replace-device-name"].as<string>();
            if(device_name_t.size() != 0) device_name = device_name_t;
        }

        if(device_name.size() == 0)
            device_name = "UNKNOWN";
        if(device_platform.size() == 0)
            device_platform = "UNKNOWN";
        if(device_toolkit.size() == 0)
            device_toolkit = "UNKNOWN";
        if(device_compute.size() == 0)
            device_compute = "UNKNOWN";

        ////////////////////////////////////////////////////////////////////////////
        // Get the current time. Strip the newline from asctime(...)
        ////////////////////////////////////////////////////////////////////////////
        time_t current_time = time(nullptr);
        struct tm * timeinfo = gmtime(&current_time);
        string local_time = asctime(localtime(&current_time));
        local_time.erase(remove(local_time.begin(), local_time.end(), '\n'), local_time.end());
        char timestamp[1024];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H_%M_%S", timeinfo);

        ////////////////////////////////////////////////////////////////////////////
        // Set filename
        ////////////////////////////////////////////////////////////////////////////
        stringstream filename;
        filename << timestamp << "_" << hostname << "_" << device_platform << "_"
                 << device_name << ".csv";

        ////////////////////////////////////////////////////////////////////////////
        // Setup table
        ////////////////////////////////////////////////////////////////////////////
        AFResultsTable::Instance().setFileName(filename.str());
        AFResultsTable::Instance().addStaticColumn("AF_VERSION", af_version);
        AFResultsTable::Instance().addStaticColumn("AF_REVISION", af_revision);
        AFResultsTable::Instance().addStaticColumn("AF_DEVICE", device_name);
        AFResultsTable::Instance().addStaticColumn("AF_PLATFORM", device_platform);
        AFResultsTable::Instance().addStaticColumn("AF_TOOLKIT", device_toolkit);
        AFResultsTable::Instance().addStaticColumn("AF_COMPUTE", device_compute);
        AFResultsTable::Instance().addStaticColumn("AF_OS", host_operating_system);
        AFResultsTable::Instance().addStaticColumn("LOCAL_TIME", local_time);
        AFResultsTable::Instance().addStaticColumn("POSIX_TIME", to_string(current_time));

        celero::AddExperimentResultCompleteFunction(
        [](shared_ptr<celero::Result> p)
        {
            AFResultsTable::Instance().add(p);
        });

        ////////////////////////////////////////////////////////////////////////////
        // Get a list of all experiments and create vector for benchmarks to run
        ////////////////////////////////////////////////////////////////////////////
        vector<pair<string,string>> all_benchmarks = getExperimentNames();
        vector<pair<string,string>> benchmarks;
        sort(all_benchmarks.begin(), all_benchmarks.end());

        ////////////////////////////////////////////////////////////////////////////
        // Run specific benchmark. Requires only partial match
        ////////////////////////////////////////////////////////////////////////////
        if(vm.count("benchmark"))
        {
            vector<string> findBenchmarkNames = vm["benchmark"].as<vector<string>>();
            for(int i = 0; i < (int)findBenchmarkNames.size(); i++) {
                string exprName = findBenchmarkNames[i];
                for(int j = 0; j < (int)all_benchmarks.size(); j++) {
                    if(all_benchmarks[j].second.find(exprName) != string::npos)
                        benchmarks.push_back(*(all_benchmarks.begin() + j));
                }
            }

            // If no experiments match, exit
            if(benchmarks.empty())
            {
                cout << "No specified benchmarks or experiments found. Exiting." << endl;
                return 0;
            }
        }

        ////////////////////////////////////////////////////////////////////////////
        // Override default problem-space
        ////////////////////////////////////////////////////////////////////////////
        if(vm.count("problem-space"))
        {
            auto overrideProblemSpace = vm["problem-space"].as<csv_option<uint64_t>>().values;
            for(uint64_t x : overrideProblemSpace){
                AF_Fixture::overrideProblemSpace.push_back(std::make_pair(x, (uint64_t) 0));
            }
            // If no experiments match, exit
            if(AF_Fixture::overrideProblemSpace.empty())
            {
                cout << "Empty problem-space specified. Exiting." << endl;
                return 0;
            }
        }

        ////////////////////////////////////////////////////////////////////////////
        // Exclude specific benchmark. Requires only partial match
        ////////////////////////////////////////////////////////////////////////////
        if(vm.count("exclude-benchmark"))
        {
            if(benchmarks.size() == 0)
                benchmarks = all_benchmarks;

            vector<string> findBenchmarkNames = vm["exclude-benchmark"].as<vector<string>>();
            for(int i = 0; i < (int)findBenchmarkNames.size(); i++) {
                string exprName = findBenchmarkNames[i];
                for(int j = benchmarks.size() - 1; j > -1; j--) {
                    if(benchmarks[j].second.find(exprName) != string::npos) {
                        benchmarks.erase(benchmarks.begin() + j);
                    }
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////////
        // Run specific group. Requires only partial match
        ////////////////////////////////////////////////////////////////////////////
        if(vm.count("group"))
        {
            vector<string> findBenchmarkNames = vm["group"].as<vector<string>>();
            for(int i = 0; i < (int)findBenchmarkNames.size(); i++) {
                string exprName = findBenchmarkNames[i];
                for(int j = 0; j < (int)all_benchmarks.size(); j++) {
                    if(all_benchmarks[j].first.find(exprName) != string::npos)
                        benchmarks.push_back(*(all_benchmarks.begin() + j));
                }
            }

            // If no experiments match, exit
            if(benchmarks.empty())
            {
                cout << "No specified benchmarks or experiments found. Exiting." << endl;
                return 0;
            }
        }

        ////////////////////////////////////////////////////////////////////////////
        // Exclude specific group. Requires only partial match
        ////////////////////////////////////////////////////////////////////////////
        if(vm.count("exclude-group"))
        {
            if(benchmarks.size() == 0)
                benchmarks = all_benchmarks;

            vector<string> findBenchmarkNames = vm["exclude-group"].as<vector<string>>();
            for(int i = 0; i < (int)findBenchmarkNames.size(); i++) {
                string exprName = findBenchmarkNames[i];
                for(int j = benchmarks.size() - 1; j > -1; j--) {
                    if(benchmarks[j].second.find(exprName) != string::npos) {
                        benchmarks.erase(benchmarks.begin() + j);
                    }
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////////
        // Start from specific group. Requires only partial match
        ////////////////////////////////////////////////////////////////////////////
        if(vm.count("start-from-group"))
        {
            vector<string> findBenchmarkNames = vm["start-from-group"].as<vector<string>>();
            for(int i = 0; i < (int)findBenchmarkNames.size(); i++) {
                string exprName = findBenchmarkNames[i];
                int pos = -1;
                for(int j = 0; j < (int)all_benchmarks.size(); j++) {
                    if(all_benchmarks[j].first.find(exprName) != string::npos) {
                        pos = j;
                        break;
                    }
                }
                // If no experiments match, exit
                if(pos == -1)
                {
                    cout << "No specified benchmarks or experiments found. Exiting." << endl;
                    return 0;
                }
                for(int j = pos; j < (int)all_benchmarks.size(); j++) {
                    benchmarks.push_back(*(all_benchmarks.begin() + j));
                }
            }

            // If no experiments match, exit
            if(benchmarks.empty())
            {
                cout << "No specified benchmarks or experiments found. Exiting." << endl;
                return 0;
            }
        }

        ////////////////////////////////////////////////////////////////////////////
        // At this point run all benchmarks if still empty
        ////////////////////////////////////////////////////////////////////////////
        if(benchmarks.size() == 0)
            benchmarks = all_benchmarks;

        ////////////////////////////////////////////////////////////////////////////
        // Disable image benchmarks if the image directory was not specified.
        ////////////////////////////////////////////////////////////////////////////
        if(!enableImageBenchmarks)
        {
            cout << "No image directory was specified, disabling image benchmarks." << endl;

            for(int i = benchmarks.size() - 1; i > -1; i--)
            {
                if(benchmarks[i].first == "Image")
                    benchmarks.erase(benchmarks.begin() + i);
            }
        }

        ////////////////////////////////////////////////////////////////////////////
        // Disable double tests based on user or device condition
        ////////////////////////////////////////////////////////////////////////////
        if(vm.count("disable-double") || !doubleAvailable)
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

        ////////////////////////////////////////////////////////////////////////////
        // Sort the benchmarks so that they are in alphabetical order
        ////////////////////////////////////////////////////////////////////////////
        sort(benchmarks.begin(), benchmarks.end());

        ////////////////////////////////////////////////////////////////////////////
        // Print selected tests
        ////////////////////////////////////////////////////////////////////////////
        if(vm.count("print"))
        {
            int fieldWidth = 24;
            cout << endl;
            cout << "List of benchmarks selected:" << endl;
            cout << " " << setw(fieldWidth + 1) << left << "Group" << "Benchmark" << endl;
            cout << " " << setw(fieldWidth + 1) << left << "---------" << "----------" << endl;

            for(auto experment_name: benchmarks)
                cout << " " << setw(fieldWidth) << left << experment_name.first << " " << experment_name.second << endl;
        }

        print::TableBanner();

        ////////////////////////////////////////////////////////////////////////////
        // Run the tests
        ////////////////////////////////////////////////////////////////////////////
        for(auto benchmark: benchmarks)
        {
            string group = benchmark.first;
            string experiment = benchmark.second;
            executor::Run(group, experiment);
        }

    } catch(std::exception &e) {
        cout << endl << e.what() << endl;
        return 1;
    }

    return 0;
}

