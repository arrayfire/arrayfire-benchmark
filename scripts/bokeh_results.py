#!/usr/bin/python

# general functionality
import glob
import numpy as np
import argparse
import os
# recordtable parsing
from celero_parser import *
# plotting
import math
import itertools
import bokeh.plotting as bplt


def unique_colors():
    # colors to use
    # From "A Colour Alphabet and the Limits of Colour Coding"
    # Green-Armytage (2010) Design & Creativity 10, 1-23
    # see http://eleanormaclure.files.wordpress.com/2011/03/colour-coding.pdf
    colors = itertools.cycle(['#F0A3FF', '#0075DC', '#993F00', '#4C005C',
        '#191919', '#005C31', '#2BCE48', '#FFCC99', '#808080', '#94FFB5',
        '#8F7C00', '#9DCC00', '#C20088', '#003380', '#FFA405', '#FFA8BB',
        '#426600', '#FF0010', '#5EF1F2','#00998F','#E0FF66','#740AFF',
        '#990000','#FFFF80','#FFFF00','#FF5000'])

    return colors

def plot_image_throughput(results, show_backend=False, show_benchmark_name=False,
    autosave=False, fmt="svg"):

    # This function only plots results from the "Image" group
    valid_groups = ['Image', 'SIFT', 'SURF', 'ORB']
    results = filter(lambda x: x['group'] in valid_groups, results)

    if len(results) == 0:
        return

    title = results[0]['group']
#    suffix = "throughput_ave_images_per_sec"
    ylabel = r"Average throughput (images / second)"
    xlabel = r"Image height (pixels, 16:9 aspect ratio)"
#
#    if show_benchmark_name:
#        title += " " + results[0]['benchmark_name']

    bplt.output_file("image_throughput.html", title=title)
    fig = bplt.figure(title=title)

    colors = unique_colors()

    # plot images/second vs. data size
    labels = list()
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

        color = colors.next()
        fig.line(x,y, legend=label, color=color, line_width=2)
        fig.circle(x, y, legend=label, color=color, fill_color="white", size=8)

    fig.xaxis.axis_label = xlabel
    fig.yaxis.axis_label = ylabel

    bplt.save(fig)
    quit()

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

            plot_image_throughput(temp, show_backend=show_backend,
				show_benchmark_name=show_benchmark_name,
				fmt=args.save_format, autosave=args.autosave)


# Run the main function if this is a top-level script:
if __name__ == "__main__":
    main()
