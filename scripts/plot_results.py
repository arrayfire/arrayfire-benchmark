#!/usr/bin/python

import glob
import numpy as np

def parse_celero_recordTable_header(header_row):

   
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
    Reads a RecordTable result file from Celero and returns a dictionary containing
    the test results. The dictionary will contain the following fields:
        group: string containing group name
        header: list containing strings from the header
        baseline: list containing the baseline measurement
        results: list containing each test result
    """

    infile = open(filename, 'r')
    
    new_test = True
    group_name = ""
    header = list()
    baseline = list()
    times = list()
    output = list()
    data_sizes = list()
    other_fields = list()
    other_fields_data = list()
    split_start = 0
    split_end = 0
    
    for line in infile:
        # strip off white space, skip blank lines
        line = line.strip("\n")

        line = line.split(',')
        
        # Tests are separated by a line with no data
        if(len(line) == 1 or (line[0] == '' and line[1] == '')):
            # save the current result
            if(len(times) > 0):
                temp = {'group': group_name, 'data_sizes': data_sizes, 
                    'times': times, 'baseline': baseline, 
                    'other_fields': other_fields, 'other_fields_data': other_fields_data}
                output.append(temp)
            # reset locals
            group_name = ""
            header = list()
            baseline = list()
            times = list()
            # mark the new test block and advance to the next line
            new_test = True
            continue
            
        # pull out the group name, it is the only thing on the line
        if(new_test):
            group_name = line[0]
            new_test = False
            # advance to the next line
            continue
        
        # parse the header row
        if(line[0] == ''):
            [data_sizes, other_fields, split_start, split_end] = parse_celero_recordTable_header(line)
        else:
            if(line[0] == "Baseline"):
                baseline = line[split_start:split_end]
                baseline = map(float, baseline)
                baseline = np.array(baseline)
            else:
                other_fields_data = line[1:split_start]

                t_times = line[split_start:split_end]
                t_times = map(float, t_times)
                t_times = np.array(t_times)
                times.append(t_times)
            
    return output
    
    
def import_directory(directory):

    csv_files = glob.glob(directory + "/*.csv")
    
    results = list()        
    
    for filename in csv_files:
        results.extend(read_celero_recordTable(filename))
        
    return results
    

results = import_directory("/home/bkloppenborg/workspace/arrayfire_benchmark/results/")



temp = list()
for result in results:
    if result['group'] == 'Accumulate_1D_f32':
        temp.append(result)
        
      
import matplotlib.pyplot as plt  
import math
import matplotlib.cm as cm

colors = cm.rainbow(np.linspace(0, 1, len(temp)))
color_id = 0

# plot data size vs. execution time
for result in temp:
    x = np.sqrt(result['data_sizes'])
    y = result['times']
    
    #print result['data_sizes']
    #print result['times']
 
    plt.scatter(x, y, color=colors[color_id], label=result['other_fields_data'][2])
   # plt.plot(x,y)
    color_id += 1

plt.legend(numpoints=1, loc='upper left')   
plt.show()

color_id = 0
for result in temp:
    x = np.sqrt(result['data_sizes'])
    y = result['data_sizes'] / result['times']

    plt.scatter(x, y, color=colors[color_id], label=result['other_fields_data'][2])
    color_id += 1
    
plt.legend(numpoints=1, loc='upper left')   
plt.show()
