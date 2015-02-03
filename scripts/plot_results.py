#!/usr/bin/python

# general functionality
import glob
import numpy as np
import argparse
import os
# plotting
import matplotlib.pyplot as plt  
import math
import matplotlib.cm as cm

class read_state():
    """A class representing an enumerated state of the Celero file reader
    """
    start = 0
    header = 1
    result_row = 2
    finished = 3

def list_recordTable_attribute(results, attribute):

    entries = list()
    for result in results:
        entries.append(result['extra_data'][attribute])
        
    unique_entries = set(entries)
    unique_entries = sorted(unique_entries)
    return unique_entries

def list_recordTable_benchmarks(results):
    """Returns a unique sorted list of all benchmarks
    """
    all_benchmarks = list()
    for result in results:
        all_benchmarks.append(result['benchmark_name'])
        
    unique_benchmarks = set(all_benchmarks)
    unique_benchmarks = sorted(unique_benchmarks)
    
    return unique_benchmarks

def list_recordTable_groups(results):
    """Returns a unique sorted list of all groups of benchmarks
    """
    all_groups = list()
    for result in results:
        all_groups.append(result['group'])
        
    unique_groups = set(all_groups)
    unique_groups = sorted(unique_groups)
    return unique_groups
   
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

def parse_celero_recordTable_header(header_row):
    """
    Parses a Celero recordTable header row separating the row into the 
    data size and other field sections. Returns a list of the following
    structures:
    
        data_sizes - np.ndarray containing the sizes of the data
        other_fields - extra fields
        split_start - where the data_sizes split begins
        split_end - where the data_sizes split ends
    
    """
   
    data_sizes = filter(lambda x: x.isdigit(), header_row)
    other_fields = filter(lambda x: not x.isdigit(), header_row)
    other_fields = filter(lambda x: x != "", other_fields)
    
    split_start = len(other_fields) + 1
    split_end = split_start + len(data_sizes)
    
    data_sizes = map(int, data_sizes)
    data_sizes = np.array(data_sizes)
    
    return [data_sizes, other_fields, split_start, split_end]


def plot_time_vs_size(results, show_backend=False, show_benchmark_name=False, 
    autosave=False, fmt="svg"):
    """ Creates a plot of execution time vs. data size
    """
    
    # exclude specific groups from being plotted by this function
    exclude_groups = ["Image"]
    results = filter(lambda x: x['group'] not in exclude_groups, results)
    
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

def plot_image_bar_chart(results, show_backend=False, show_benchmark_name=False, 
    autosave=False, fmt="svg"):

    # This function only plots results from the "Image" group
    results = filter(lambda x: x['group'] == "Image", results)

    if len(results) == 0:
        return

    colors = cm.rainbow(np.linspace(0, 1, len(results)))

    # plot images/second vs. data size
    x_id = 0
    width = 0.75
    labels = list()
    for result in results:
      
        # extract results 
        ave_time = result['times'] / result['data_sizes'] 
        # construct the label
        label = result['extra_data']['AF_DEVICE']
        if show_backend:
            label += " " + result['extra_data']['AF_PLATFORM']
        labels.append(label)

        # plot this entry
        plt.bar(x_id + width/2, result['times'], color=colors[x_id], align='center')
        
        x_id += 1

    # set the title
    title = results[0]['group']
    suffix = "throughput_ave_images_per_sec"
    ylabel = r"Average throughput (images / second)"
    xlabel = ""
    
    if show_benchmark_name:
        title += " " + results[0]['benchmark_name']

    # setup the x-labels to match the labels of the individual plots
    ind = np.arange(x_id)
    plt.xticks(ind + width / 2, labels)
    label_and_plot(title, suffix, ylabel, xlabel, autosave=autosave, fmt=fmt)   
    
    

def read_celero_recordTable(filename):
    """
    Splits a group of Celero test results into individual results which can
    be plotted.
    """
    
    infile = open(filename, 'r')
    
    # The results are structured as follows
    #  group_name0,...
    #  '',extra_0,...,extraN,size0,...,sizeN
    #  benchmark0,extra0,...,extraN,time0,...,timeN
    #  ...
    #  benchmarkM,extra0,...,extraN,time0,...,timeN
    #  '',... (or otherwise blank)
    #  group_name1,...
    
    # output variables
    output = list()
    # state and data variables
    state = read_state.start
    group = ""
    data_sizes = list()
    other_fields = list()
    split_start = 0
    split_end = 0
    
    for line in infile:
    
        # End of a group, clear variables
        if state == read_state.finished:
            group = ""
            data_sizes = list()
            other_fields = list()
            split_start = 0
            split_end = 0
            # reset the read state
            state = read_state.start
            
        # strip off newline, split the line, remove empty fields
        line = line.strip("\n")
        line = line.split(',')
        line = filter(lambda x: x != "", line)      
        
        # check for the end of the group
        if len(line) == 0:
            state = read_state.finished
            continue
            
        # group ID row
        if state == read_state.start:
            group_name = line[0]
            state = read_state.header
            continue
            
        # header row
        if state == read_state.header:
            [data_sizes, other_fields, split_start, split_end] = parse_celero_recordTable_header(line)
            state = read_state.result_row
            continue
            
        # result row
        if state == read_state.result_row:
            benchmark_name = line[0]
            
            other_data = line[1:split_start]
            t_times = line[split_start:split_end]
            t_times = map(float, t_times)
            t_times = np.array(t_times)
            
            extra_data = dict(zip(other_fields, other_data))
            
            result = {'group': group_name, 'benchmark_name': benchmark_name, 
                'data_sizes': data_sizes, 'times': t_times,
                'extra_data': extra_data}
            
            output.append(result)
        
    return output
    
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
            plot_image_bar_chart(temp, show_backend=show_backend, 
				show_benchmark_name=show_benchmark_name, 
				fmt=args.save_format, autosave=args.autosave)
         
    
# Run the main function if this is a top-level script:
if __name__ == "__main__":
    main()
