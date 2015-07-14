#!/usr/bin/python

# general functionality
import glob
import numpy as np
import argparse
import os
# recordtable parsing
from celero_parser import *
# plotting
import matplotlib.pyplot as plt
import math
import matplotlib.cm as cm

def label_and_plot(title, suffix, ylabel, xlabel, autosave=False, fmt='svg'):
    """Sets the title, x/y labels, and x/y limits. Plots the data or
       saves the file
    """

    # set specific plot options
    plt.xlim(0,plt.xlim()[1])
    plt.ylim(0,plt.ylim()[1])
    plt.title(title)
    plt.ylabel(ylabel)
    plt.xlabel(xlabel)

    if autosave:
        plt.savefig(title + '_' + suffix + '.' + fmt, bbox_inches='tight')
        plt.close()
    else:
        plt.show()

def plot_time_vs_size(results, show_backend=False, show_benchmark_name=False,
    autosave=False, fmt="svg"):
    """ Creates a plot of execution time vs. data size
    """

    # exclude specific groups from being plotted by this function
    exclude_groups = ["Image"]
    results = filter(lambda x: x['group'] not in exclude_groups, results)

    if len(results) == 0:
        return

    colors = cm.rainbow(np.linspace(0, 1, len(results)))

    # plot execution time vs. data size
    color_id = 0
    for result in results:
        x = np.sqrt(result['data_sizes'])
        y = result['times']

        # construct the label
        label = result['extra_data']['AF_DEVICE']
        if show_backend:
            label += " " + result['extra_data']['AF_PLATFORM']

        plt.scatter(x, y, color=colors[color_id], label=label, linewidth=2.0)
        plt.plot(x,y, color=colors[color_id], label=None, linewidth=2.0)
        color_id += 1

    # set specific plot options
    title = results[0]['group']
    suffix = "execution_time"
    ylabel = "Execution time (micro-seconds)"
    xlabel = "Image width"

    if show_benchmark_name:
        title += " " + results[0]['benchmark_name']

    plt.legend(loc='upper left', numpoints=1)
    label_and_plot(title, suffix, ylabel, xlabel, autosave=autosave, fmt=fmt)

def plot_throughput_vs_size(results, show_backend=False, show_benchmark_name=False,
    autosave=False, fmt="svg"):

    # exclude specific groups from being plotted by this function
    exclude_groups = ["Image"]
    results = filter(lambda x: x['group'] not in exclude_groups, results)

    if len(results) == 0:
        return

    colors = cm.rainbow(np.linspace(0, 1, len(results)))

    # plot for throughput vs. data size
    color_id = 0
    for result in results:
        x = np.sqrt(result['data_sizes'])
        # display the plot in billions of elements per second (1E9)
        y = result['data_sizes'] / ((result['times'] + 1E-10)* 1E-6) / 1E9

        # construct the label
        label = result['extra_data']['AF_DEVICE']
        if show_backend:
            label += " " + result['extra_data']['AF_PLATFORM']

        plt.scatter(x, y, color=colors[color_id], label=label, linewidth=2.0)
        plt.plot(x,y, color=colors[color_id], label=None, linewidth=2.0)
        color_id += 1

    # set specific plot options
    title = results[0]['group']
    suffix = "throughput_elements_per_sec"
    ylabel = r"Throughput ($10^9$ elements / second)"
    xlabel = "Image width"

    if show_benchmark_name:
        title += " " + results[0]['benchmark_name']

    plt.legend(loc='upper left', numpoints=1)
    label_and_plot(title, suffix, ylabel, xlabel, autosave=autosave, fmt=fmt)

def plot_image_rate_vs_size(results, show_backend=False, show_benchmark_name=False,
    autosave=False, fmt="svg"):

    # exclude specific groups from being plotted by this function
    exclude_groups = ["Image"]
    results = filter(lambda x: x['group'] not in exclude_groups, results)

    if len(results) == 0:
        return

    colors = cm.rainbow(np.linspace(0, 1, len(results)))

    # plot images/second vs. data size
    color_id = 0
    for result in results:
        x = np.sqrt(result['data_sizes'])
        # display log plot in thousands of images per second. Add a small
        # delta to force log plot to always be greater than zero
        y = 1.0/((result['times'] + 1E-10)* 1E-6)

        # construct the label
        label = result['extra_data']['AF_DEVICE']
        if show_backend:
            label += " " + result['extra_data']['AF_PLATFORM']

        plt.scatter(x, y, color=colors[color_id], label=label, linewidth=2.0)
        plt.plot(x,y, color=colors[color_id], label=None, linewidth=2.0)
        color_id += 1

    title = results[0]['group']
    suffix = "throughput_images_per_sec"
    ylabel = r"Throughput (images / second)"
    xlabel = "Image width"

    if show_benchmark_name:
        title += " " + results[0]['benchmark_name']

    plt.xscale('log')
    plt.yscale('log')
    plt.legend(loc='upper right', numpoints=1)
    label_and_plot(title, suffix, ylabel, xlabel, autosave=autosave, fmt=fmt)

def plot_image_throughput(results, show_backend=False, show_benchmark_name=False,
    autosave=False, fmt="svg"):

    # This function only plots results from the "Image" group
    valid_groups = ['Image', 'SIFT', 'SURF', 'ORB']
    results = filter(lambda x: x['group'] in valid_groups, results)

    if len(results) == 0:
        return

    colors = cm.rainbow(np.linspace(0, 1, len(results)))

    # plot images/second vs. data size
    labels = list()
    color_id = 0;
    for result in results:

        # extract results
        x = result['data_sizes']
        y = 1.0 / (result['times'] * 1E-6)
        # construct the label
        label = result['extra_data']['AF_DEVICE']
        if show_backend:
            label += " " + result['extra_data']['AF_PLATFORM']

        if 'AF_LABEL' in result['extra_data'].keys():
            label = result['extra_data']['AF_LABEL']

        labels.append(label)

        plt.scatter(x, y, color=colors[color_id], label=label, linewidth=2.0)
        plt.plot(x,y, color=colors[color_id], label=None, linewidth=2.0)

        color_id += 1

    # set the title
    title = results[0]['group']
    suffix = "throughput_ave_images_per_sec"
    ylabel = r"Average throughput (images / second)"
    xlabel = r"Image height (pixels, 16:9 aspect ratio)"

    if show_benchmark_name:
        title += " " + results[0]['benchmark_name']

    # setup the x-labels to match the labels of the individual plots
    plt.legend(loc='upper right', numpoints=1)
    label_and_plot(title, suffix, ylabel, xlabel, autosave=autosave, fmt=fmt)


def import_directory(directory):
    """
    Creates a list of all .csv files in a directory, imports them using
    read_celero_recordTable, and returns the result.
    """

    csv_files = glob.glob(directory + "/*.csv")

    results = list()

    for filename in csv_files:
        results.extend(read_celero_recordTable(filename))

    return results

def main():

    # parse command-line arguments
    parser = argparse.ArgumentParser()
    # general arguments / functionality for any recordTable result
    parser.add_argument('files', nargs='+',
        help="Parse these Celero RecordTable files for displaying their data." +
            "This can also be directories containing RecordTable files.")
    parser.add_argument("-lg", "--list-groups",
        help="List all test groups found in the file/directory",  action="store_true")
    parser.add_argument("-g", "--groups", action='append',
        help="Show results for specific groups (may be combined with -t)")
    parser.add_argument("--autosave",
        help="Set to enable automatic saving of plots",  action="store_true", default=False)
    parser.add_argument("--save-format",
        help="Sets the format for saved files. [default: svg]", default="svg")

    # arguments specific to the ArrayFire's benchmarking
    parser.add_argument("-t", "--data-type",
        help="Show results only for a specific data type [f32, f64]")
    parser.add_argument("-lb", "--list-backends", action="store_true",
        help="Lists the backends found in the tests")
    parser.add_argument("-b", "--backend", action='append',
        help="Show plots for specific backends", default=[])
    parser.add_argument("-ld", "--list-devices", action="store_true",
        help="Lists the devices found in the tests")
    parser.add_argument("-d", "--device", action='append',
        help="Show plots for specific devices", default=[])
    parser.add_argument("-lr", "--list-revisions", action="store_true",
        help="Lists the ArrayFire revisions found in the tests")
    parser.add_argument("-r", "--revisions", action='append',
        help="Show plots for specific revisions", default=[])
    args = parser.parse_args()

    # import the data
    results = list()
    for file_or_directory in args.files:
        if os.path.isfile(file_or_directory):
            results.extend( read_celero_recordTable(file_or_directory) )
        else:
            results.extend( import_directory(file_or_directory) )

    ###
    # Lists: Requested information via. command line arguments
    ###
    # list groups of benchmarks then exit
    if args.list_groups:
        groups = list_recordTable_groups(results)
        for entry in groups:
            print(entry)
        quit()

    # list backends found in the data, then exit
    if args.list_backends:
        backends = list_recordTable_attribute(results, 'AF_PLATFORM')
        print backends
        quit()

    # list all devices found in the data, then exit
    if args.list_devices:
        devices = list_recordTable_attribute(results, 'AF_DEVICE')
        print devices
        quit()

    # list all devices found in the data, then exit
    if args.list_revisions:
        revisions = list_recordTable_attribute(results, 'AF_REVISION')
        print revisions
        quit()

    ###
    # Apply command-line specified arguments, limiting what data will be plotted
    ###
    # Limit what will be plotted by explicitly including the arguments
    # specified on the command line
    include_groups = list()
    all_groups = list_recordTable_groups(results)
    # include specific data types
    if args.data_type:
        include_groups = filter(lambda x: x[-3:] == args.data_type, all_groups)
    # include specific tests
    if args.groups:
        include_groups.extend(args.groups)
    # If no specific options have been set, include all groups by default
    if len(include_groups) == 0:
        include_groups = all_groups
    # limit by groups
    results = filter(lambda x: x['group'] in include_groups, results)

    # limit by backend
    if len(args.backend) > 0:
        results = filter(lambda x: x['extra_data']['AF_PLATFORM'] in args.backend, results)
    # indicate whether or not we should show the backend name on the plots
    show_backend = False
    if len(args.backend) > 1:
        show_backend = True

    # limit by device type
    if len(args.device) > 0:
        results = filter(lambda x: x['extra_data']['AF_DEVICE'] in args.device, results)

    # make the plots
    groups = list_recordTable_groups(results)
    for group in groups:
        # extract only the results which match this group
        filtered_results = filter(lambda x: x['group'] == group, results)
        # remove the baseline measurements from the plots
        filtered_results = filter(lambda x: x['benchmark_name'] != "Baseline", filtered_results)

        # get a list of all benchmarks remaining
        benchmarks = list_recordTable_benchmarks(filtered_results)

        show_benchmark_name = False
        if len(benchmarks) > 1:
            show_benchmark_name = True

        for benchmark in benchmarks:

            # plot one benchmark at a time
            temp = filter(lambda x: x['benchmark_name'] == benchmark, filtered_results)

#            plot_time_vs_size(temp, show_backend=show_backend,
#				show_benchmark_name=show_benchmark_name,
#				fmt=args.save_format, autosave=args.autosave)
#            plot_throughput_vs_size(temp, show_backend=show_backend,
#				show_benchmark_name=show_benchmark_name,
#				fmt=args.save_format, autosave=args.autosave)
#            plot_image_rate_vs_size(temp, show_backend=show_backend,
#				show_benchmark_name=show_benchmark_name,
#				fmt=args.save_format, autosave=args.autosave)
            plot_image_throughput(temp, show_backend=show_backend,
				show_benchmark_name=show_benchmark_name,
				fmt=args.save_format, autosave=args.autosave)


# Run the main function if this is a top-level script:
if __name__ == "__main__":
    main()
