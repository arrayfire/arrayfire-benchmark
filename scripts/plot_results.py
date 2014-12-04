#!/usr/bin/python

def parse_celero_recordTable_header(header_row):

   
    data_sizes = filter(lambda x: x.isdigit(), header_row)
    other_fields = filter(lambda x: not x.isdigit(), header_row)
    other_fields = filter(lambda x: x != "", other_fields)
    
    split_start = len(other_fields) + 1
    split_end = split_start + len(data_sizes)
    
    data_sizes = map(int, data_sizes)
    
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
    results = list()
    output = list()
    data_sizes = list()
    other_fields = list()
    other_fields_data = list()
    split_start = 0
    split_end = 0
    
    for line in infile:
        # strip off white space, split on commas, remove empty entries
        line = line.strip("\n")
        line = line.split(',')
        
        # Tests are separated by a line with no data
        if(line[0] == '' and line[1] == ''):
            # save the current result
            if(len(results) > 0):
                temp = {'group': group_name, 'data_sizes': data_sizes, 
                    'results': results, 'baseline': baseline, 
                    'other_fields': other_fields, 'other_fields_data': other_fields_data}
                output.append(temp)
            # reset locals
            group_name = ""
            header = list()
            baseline = list()
            results = list()
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
            else:
                other_fields_data = line[1:split_start]

                t_results = line[split_start:split_end]
                t_results = map(float, t_results)
                results.append(t_results)
            
    return output
    
results = list()        
results.extend(read_celero_recordTable("../results/ripley_GTX_750_cuda.csv"))
results.extend(read_celero_recordTable("../results/romulus_GTX_780_cuda.csv"))

temp = list()
for result in results:
    if result['group'] == 'Accumulate_1D_f32':
        temp.append(result)
        
print temp
    

