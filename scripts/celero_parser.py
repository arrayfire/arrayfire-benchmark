# general functionality
import numpy as np

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
        line = line.strip("\r")
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
            try:
                t_times = map(float, t_times)
            except:
                print "Failed to parse line in " + filename + "\n"
                print line
                print t_times
                exit()

            t_times = np.array(t_times)

            extra_data = dict(zip(other_fields, other_data))

            result = {'group': group_name, 'benchmark_name': benchmark_name,
                'data_sizes': data_sizes, 'times': t_times,
                'extra_data': extra_data}

            output.append(result)

    return output


