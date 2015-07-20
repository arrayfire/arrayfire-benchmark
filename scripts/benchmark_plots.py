#!/bin/python

# Celero recordtable parser
import glob
import os
from celero_parser import *

# Bokeh components
from bokeh.plotting import figure, output_server, cursession, show
from bokeh.models import Plot, ColumnDataSource, HoverTool

def make_field_ids(id_number):
    i = str(id_number)
    y_id = 'y' + i
    device_id = 'device' + i
    platform_id = 'platform' + i

    return [y_id, device_id, platform_id]

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

def getXY(celero_result, axis_filter):
    """Returns the X or Y value as specified by axis_filter"""

    # TODO: Remove the baseline measurement from the timing results
    if axis_filter == 'size':
        return celero_result['data_sizes']
    if axis_filter == 'log10(size)':
        return np.log10(celero_result['data_sizes'])
    if axis_filter == 'log2(size)':
        return np.log2(celero_result['data_sizes'])
    elif axis_filter == 'time [ms]':
        return celero_result['times'] * 1E-3
    elif axis_filter == 'throughput [1/sec]':
        return 1.0 / (celero_result['times'] * 1E-6)

##
# Define the data sources
##

MAX_PLOTS = 4
# define x/y axis possibilities
# NOTE: If you change an option here, change it in getXY above too
axis_options = ['size', 'log10(size)', 'log2(size)', 'time [ms]', 'throughput [1/sec]']

# Parse the data directory
data_dir = "/home/bkloppenborg/workspace/arrayfire_benchmark/results"
celero_results = import_directory(data_dir)

# Get a list of all of the benchmarks
benchmark_names = list_recordTable_benchmarks(celero_results)
benchmark_names = filter(lambda x: x != "Baseline", benchmark_names)

platform_names = list_recordTable_attribute(celero_results, 'AF_PLATFORM')
device_names = list_recordTable_attribute(celero_results, 'AF_DEVICE')

##
# Configure the data
##

benchmark = "JIT_ADD"
devices = ["GeForce_GTX_780"]
platforms = ["CUDA"]

# extract only the results which match this group
filtered_results = filter(lambda x: x['benchmark_name'] == benchmark, celero_results)
# remove the baseline measurements from the plots
filtered_results = filter(lambda x: x['benchmark_name'] != "Baseline", filtered_results)

# select the desired devices
filtered_results = filter(lambda x: x['extra_data']['AF_DEVICE'] in devices, filtered_results)
filtered_results = filter(lambda x: x['extra_data']['AF_PLATFORM'] in platforms, filtered_results)

source = ColumnDataSource(data=dict(x=[], y=[]))

##
# Make the plot
##
output_server("animated_line")

y_id, device_id, platform_id = make_field_ids(0)

result = filtered_results[0]

# Extract the results from the benchmark
platform = result['extra_data']['AF_PLATFORM']
device = result['extra_data']['AF_DEVICE']

x = getXY(result, "size")
y = getXY(result, "size")

#logging.debug("Params: %s %s", benchmark, devices)

# store the benchmark results in the self.source object
# NOTE: we replicate the device and platform data here so that
# it works correctly with the mouseover/hover
source.data['x'] = x
source.data['y'] = y
source.data['device'] = [device] * len(x)
source.data['platform'] = [platform] * len(x)
source._dirty = True

##
# Configure the plot area
##

plot = figure(title_text_font_size="12pt",
    plot_height=400,
    plot_width=400,
#    tools=toolset,
#    title=title,
)
# remove the logo
plot.logo = None


plot.line(   'x', 'y', source=source, line_color="red",
    line_width=3, line_alpha=0.6)
plot.scatter('x', 'y', source=source, fill_color="white", size=8)

show(plot)

