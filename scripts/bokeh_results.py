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
from bokeh.models import HoverTool
from bokeh.models.widgets import Select
from bokeh.io import output_file, save, vform

# valid types for the axes
axis_options = ['time', 'size', 'log2size', 'log10size',
    'throughput', 'log2throughput', 'log10throughput']

def format_data(benchmark, axis_type):

    times = benchmark['times'] * 1E-6
    sizes = benchmark['data_sizes']

    data = list()
    label = axis_type

    # Valid axis types as found in `axis_options`
    if axis_type == 'time':
        data = times
        label = "Time (ms)"
    if axis_type == 'size':
        data = sizes
        label = "Size"
    if axis_type == 'log2size':
        data = np.log2(sizes)
    if axis_type == 'log10size':
        data = np.log10(sizes)
    if axis_type == 'throughput':
        data = 1.0 / times
        label = "Throughput (1 / sec)"
    if axis_type == 'log2throughput':
        data = np.log2(1.0 / times)
        label = "Throughput (log2(1/sec))"
    if axis_type == 'log10throughput':
        data = np.log10(1.0 / times)
        label = "Throughput (log10(1/sec))"

    return data, label

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

def filter_benchmarks(all_benchmarks, include_benchmarks, exclude_benchmarks,
    include_groups, exclude_groups,
    include_devices, include_backends, include_revisions, include_data_types):

    filtered_benchmarks = list()
    # Generate a few booleans to speed up comps in the loop below
    filter_ib = len(include_benchmarks) > 0
    filter_eb = len(exclude_benchmarks) > 0
    filter_ig = len(include_groups) > 0
    filter_eg = len(exclude_groups) > 0
    filter_id = len(include_devices) > 0
    filter_ibe = len(include_backends) > 0
    filter_ir = len(include_revisions) > 0
    filter_idt = len(include_data_types) > 0

    for benchmark in all_benchmarks:
        # benchmarks are Python dicts with the following fileds
        # group, benchmark_name, data_sizes, times
        # extra_data = {...}

        # remove baseline measurements
        if benchmark['benchmark_name'] == "Baseline":
            continue

        if filter_ib and benchmark['benchmark_name'] not in include_benchmarks:
            continue

        if filter_eb and benchmark['benchmark_name'] in exclude_benchmarks:
            continue

        if filter_ig and benchmark['group'] not in include_groups:
            continue

        if filter_eg and benchmark['group'] in exclude_groups:
            continue

        if filter_id and benchmark['extra_data']['AF_DEVICE'] not in include_devices:
            continue

        if filter_ibe and benchmark['extra_data']['AF_PLATFORM'] not in include_backends:
            continue

        if filter_ir and benchmark['extra_data']['AF_REVISION'] not in include_revisions:
            continue

        # All Benchmark functions are prefixed by 3-character data types
        if filter_idt and x[-3:] not in include_data_types:
            continue

        filtered_benchmarks.append(benchmark)

    return filtered_benchmarks

def import_directory(directory):
    """
    Creates a list of all .csv files in a directory, imports them using
    read_celero_recordTable, and returns the result.
    """

    csv_files = glob.glob(directory + "/*.csv")

    benchmarks = list()

    for filename in csv_files:
        benchmarks.extend(read_celero_recordTable(filename))

    return benchmarks

def plot_benchmark(savefile, benchmarks, title, xaxis_type, yaxis_type):

    # configure the colors
    colors = unique_colors()

    # configure the hover box
    hover = HoverTool(
        tooltips = [
            ("Device", "@device"),
            ("Backend", "@platform"),
            ("(x,y)", "(@x,@y)")
        ])

    # configure the plot title and axis labels
    bplt.output_file(savefile + ".html", title=title)
    plot = bplt.figure(title=title, tools=[hover,'save,box_zoom,resize,reset'])
    xlabel = ""
    ylabel = ""

    # plot images/second vs. data size
    for benchmark in benchmarks:
        # get the color we will use for this plot
        color = colors.next()

        # extract benchmarks
        x,xlabel = format_data(benchmark, xaxis_type)
        y,ylabel = format_data(benchmark, yaxis_type)
        platform = benchmark['extra_data']['AF_PLATFORM']
        # get the device name, override if necessary
        device = benchmark['extra_data']['AF_DEVICE']
        if 'AF_LABEL' in benchmark['extra_data'].keys():
            device = benchmark['extra_data']['AF_LABEL']

        source = bplt.ColumnDataSource(
            data = dict(x=x,y=y,
                device=[device]*len(x),
                platform=[platform]*len(x),
            ))

        # generate the plot
        plot.line(x,y, legend=device, color=color, line_width=2)
        plot.scatter('x', 'y', source=source, legend=device, color=color,
            fill_color="white", size=8)

    plot.xaxis.axis_label = xlabel
    plot.yaxis.axis_label = ylabel

    # save the plot
    bplt.save(plot)

def main():

    # parse command-line arguments
    parser = argparse.ArgumentParser()
    # general arguments / functionality for any recordTable result
    parser.add_argument('files', nargs='+',
        help="ArrayFire benchmark RecordTable files to be parsed." +
            "This may also be a directory containing RecordTable files.")

    # Arguments which control the plot output
    parser.add_argument("-lb", "--list-benchmarks", action="store_true",
        help="List all benchmarks found in the files.")
    parser.add_argument("-b", "--benchmark", action='append', default=[],
        help="Generate a plot for this benchmark")
    parser.add_argument("-e", "--exclude-benchmark", action='append', default=[],
        help="Exclude a specific benchmark from being plotted.")

    parser.add_argument("-g", "--group", action='append', default=[],
        help="Generate plots from this group of benchmarks")
    parser.add_argument("-x", "--exclude-group", action='append', default=[],
        help="Exclude a specific group of benchmarks from being plotted")

    parser.add_argument("-ld", "--list-devices", action="store_true",
        help="List all devices found in the benchmark files")
    parser.add_argument("-d", "--device", action='append', default=[],
        help="Generate plots for this device")

    parser.add_argument("-la", "--list-backends", action="store_true",
        help="List all backends found in the benchmark files")
    parser.add_argument("-a", "--backend", action='append', default=[],
        help="Generate plots for this backend")

    parser.add_argument("-lt", "--list-types", action="store_true",
        help="List all data types found in the benchmark files")
    parser.add_argument("-t", "--data-type", action='append', default=[],
        help="Generate plots for this data type [f32, f64]")

    parser.add_argument("-p", "--print-benchmarks", action="store_true",
        help="Print a list of benchmarks that will be plotted before running")

    parser.add_argument("-lr", "--list-revisions", action="store_true",
        help="Lists the ArrayFire revisions found in the tests")
    parser.add_argument("-r", "--revision", action='append',
        help="Generate plots for this revision", default=[])

#    parser.add_argument("--autosave",
#        help="Automatically save plots",  action="store_true", default=False)
#    parser.add_argument("--save-format",
#        help="Sets the format for saved files. [html, jpeg, svg]", default="html")
    parser.add_argument("--custom-title",
        help="Set a custom title for the plot")

    # TODO: enforce the axis_options options
    axis_options_str = ' '.join(axis_options)
    parser.add_argument("--xaxis", default=['size'],
        help="Set the x-axis data type [" + axis_options_str + ']')
    parser.add_argument("--yaxis", default=['throughput'],
        help="Set the x-axis data type [" + axis_options_str + ']')

    args = parser.parse_args()

    # import the data
    benchmarks = list()
    for file_or_directory in args.files:
        if os.path.isfile(file_or_directory):
            benchmarks.extend( read_celero_recordTable(file_or_directory) )
        else:
            benchmarks.extend( import_directory(file_or_directory) )

    ###
    # Lists: Requested information via. command line arguments
    ###
    # list groups of benchmarks then exit
    if args.list_benchmarks:
        groups = list_recordTable_benchmarks(benchmarks)
        for entry in groups:
            print(entry)
        quit()

    # list backends found in the data, then exit
    if args.list_backends:
        backends = list_recordTable_attribute(benchmarks, 'AF_PLATFORM')
        print backends
        quit()

    # list all devices found in the data, then exit
    if args.list_devices:
        devices = list_recordTable_attribute(benchmarks, 'AF_DEVICE')
        print devices
        quit()

    # list all devices found in the data, then exit
    if args.list_revisions:
        revisions = list_recordTable_attribute(benchmarks, 'AF_REVISION')
        print revisions
        quit()

    # Apply any command-line argument filters
    benchmarks = filter_benchmarks(benchmarks,
        args.benchmark, args.exclude_benchmark,
        args.group, args.exclude_group,
        args.device, args.backend, args.revision, args.data_type)

    # Let the user see the filtered results
    if args.print_benchmarks:
        print benchmarks
        quit()

    # Now get ready to plot. Each benchmark will generate one output
    plot_benchmarks = list_recordTable_benchmarks(benchmarks)

    for benchmark in plot_benchmarks:
        # Get the benchmarks we will plot
        filtered_benchmarks = filter(lambda x: x['benchmark_name'] == benchmark, benchmarks)

        title = benchmark
        if len(args.custom_title) > 0:
            title = args.custom_title

        plot_benchmark(benchmark, filtered_benchmarks, title, args.xaxis, args.yaxis)



# Run the main function if this is a top-level script:
if __name__ == "__main__":
    main()
