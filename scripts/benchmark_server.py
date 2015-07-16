
# bokeh includes/configuration
import logging
logging.basicConfig(level=logging.DEBUG)
from bokeh.plotting import figure, Figure
from bokeh.models import Plot, ColumnDataSource, HoverTool
from bokeh.properties import Instance
from bokeh.server.app import bokeh_app
from bokeh.server.utils.plugins import object_page
from bokeh.models.widgets import HBox, Select, VBoxForm, CheckboxGroup
from bokeh.models.widgets import DataTable, TableColumn

# Celero recordtable parser
import glob
import os
from celero_parser import *

# plotting
import numpy as np

class BenchmarkApp(HBox):
    """An example of a browser-based, interactive plot with slider controls."""

    extra_generated_classes = [["BenchmarkApp", "BenchmarkApp", "HBox"]]

    inputs = Instance(VBoxForm)

    # widgets
    benchmarks = Instance(Select)
    x_axis_options = Instance(Select)
    y_axis_options = Instance(Select)
    # TODO: Convert this to a MultiSelect once it is fixed
    # https://github.com/bokeh/bokeh/issues/2495
    device_names = Instance(CheckboxGroup)
    data_display = Instance(DataTable)

    # plot and interaction
    plot = Instance(Plot)
    hover = Instance(HoverTool)
    # data
    source = Instance(ColumnDataSource)

    @classmethod
    def create(cls):
        """One-time creation of app's objects.

        This function is called once, and is responsible for
        creating all objects (plots, datasources, etc)
        """
        obj = cls()

        # define columns for the table
        columns = list()
        columns.append(TableColumn(field='x', title='x'))

        # set up the data source
        obj.source = ColumnDataSource(data=dict())
        for i in range(0, MAX_PLOTS):
            # configure field names
            y_id, device_id, platform_id = BenchmarkApp.make_field_ids(i)
            obj.source.data['x'] = []
            obj.source.data[y_id] = []
            obj.source.data[device_id] = []
            obj.source.data[platform_id] = []

            # set up columns for the table
            columns.append(TableColumn(field=y_id, title=y_id))
            columns.append(TableColumn(field=device_id, title=device_id))
            columns.append(TableColumn(field=platform_id, title=platform_id))


        # setup user input
        obj.x_axis_options = Select(title="X:", value=axis_options[0], options=axis_options)
        obj.y_axis_options = Select(title="Y:", value=axis_options[2], options=axis_options)
        obj.benchmarks = Select(title="Benchmark:", value=benchmark_names[0],
            options=benchmark_names)
        obj.device_names = CheckboxGroup(labels=device_names, active=[0])

        obj.data_display = DataTable(source=obj.source, columns=columns)

        # configure the toolset
        toolset = ['hover,save,box_zoom,resize,reset']

        title = obj.benchmarks.value + " " + \
            "(" + obj.y_axis_options.value + " vs." + obj.x_axis_options.value + ")"

        plot = figure(title_text_font_size="12pt",
            plot_height=400,
            plot_width=400,
            tools=toolset,
            title=title,
        )

        # Generate a figure container
        # Plot the line by the x,y values in the source property
        for i in range(0, MAX_PLOTS):

            y_id, device_id, platform_id = BenchmarkApp.make_field_ids(i)

            plot.line(   'x', y_id, source=obj.source, line_width=3, line_alpha=0.6)
            s = plot.scatter('x', y_id, source=obj.source, fill_color="white", size=8)
            s.select(dict(type=HoverTool)).tooltips = {
                "Device": "@" + device_id,
                "Backend": "@" + platform_id,
                "(x,y)": "(@x,@" + y_id + ")"
            }

        obj.plot = plot
        obj.update_data()

        obj.inputs = VBoxForm(
            children=[obj.benchmarks, obj.device_names,
                obj.x_axis_options, obj.y_axis_options
            ]
        )

        obj.children.append(obj.inputs)
        obj.children.append(obj.plot)

        return obj

    def setup_events(self):
        """Attaches the on_change event to the value property of the widget.

        The callback is set to the input_change method of this app.
        """
        super(BenchmarkApp, self).setup_events()
        if not self.benchmarks:
            return

        # Event registration for everything except checkboxes
        self.benchmarks.on_change('value', self, 'input_change')
        self.x_axis_options.on_change('value', self, 'input_change')
        self.y_axis_options.on_change('value', self, 'input_change')

        self.device_names.on_click(self.checkbox_handler)

    def checkbox_handler(self, active):

        self.update_data()

    def input_change(self, obj, attrname, old, new):
        """Executes whenever the input form changes.

        It is responsible for updating the plot, or anything else you want.

        Args:
            obj : the object that changed
            attrname : the attr that changed
            old : old value of attr
            new : new value of attr
        """
        self.update_data()

        # update the title
        self.plot.title = self.benchmarks.value + " " + \
            "(" + self.y_axis_options.value + " vs." + self.x_axis_options.value + ")"

    def getXY(self, celero_result, axis_filter):
        """Returns the X or Y value as specified by axis_filter"""

        # TODO: Remove the baseline measurement from the timing results
        if axis_filter == 'size':
            return celero_result['data_sizes']
        elif axis_filter == 'time [s]':
            return celero_result['times'] * 1E-6
        elif axis_filter == 'throughput [1/sec]':
            return 1.0 / (celero_result['times'] * 1E-6)

    @classmethod
    def make_field_ids(self, id_number):
        i = str(id_number)
        y_id = 'y' + i
        device_id = 'device' + i
        platform_id = 'platform' + i

        return [y_id, device_id, platform_id]


    def update_data(self):
        """Called each time that any watched property changes.

        This updates the sin wave data with the most recent values of the
        sliders. This is stored as two numpy arrays in a dict into the app's
        data source property.
        """

        # apply filters based upon the user's selection
        benchmark = self.benchmarks.value
        devices = list(device_names[i] for i in self.device_names.active)

        # extract only the results which match this group
        filtered_results = filter(lambda x: x['benchmark_name'] == benchmark, celero_results)
        # remove the baseline measurements from the plots
        filtered_results = filter(lambda x: x['benchmark_name'] != "Baseline", filtered_results)

        # select the desired devices
        filtered_results = filter(lambda x: x['extra_data']['AF_DEVICE'] in devices, filtered_results)
        filtered_results = filter(lambda x: x['extra_data']['AF_PLATFORM'] == "CUDA", filtered_results)

        self.source.data = dict()
        result_number = 0
        for result in filtered_results:
            # ensure we don't plot too many results
            if result_number > MAX_PLOTS:
                break

            y_id, device_id, platform_id = self.make_field_ids(result_number)

            # Extract the results from the benchmark
            platform = result['extra_data']['AF_PLATFORM']
            device = result['extra_data']['AF_DEVICE']

            x = self.getXY(result, self.x_axis_options.value)
            y = self.getXY(result, self.y_axis_options.value)

            logging.debug("Params: %s %s", benchmark, devices)

            # store the benchmark results in the self.source object
            # NOTE: we replicate the device and platform data here so that
            # it works correctly with the mouseover/hover
            self.source.data['x'] = x
            self.source.data[y_id] = y
            self.source.data[device_id] = [device] * len(x)
            self.source.data[platform_id] = [platform] * len(x)

            # increment the counter
            result_number += 1

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

# define x/y axis possibilities
# NOTE: If you change an option here, change it in getXY above too
axis_options = ['size', 'time [s]', 'throughput [1/sec]']

# Parse the data directory
data_dir = "/home/bkloppenborg/workspace/arrayfire_benchmark/results"
celero_results = import_directory(data_dir)

# Get a list of all of the benchmarks
benchmark_names = list_recordTable_benchmarks(celero_results)
benchmark_names = filter(lambda x: x != "Baseline", benchmark_names)

device_names = list_recordTable_attribute(celero_results, 'AF_DEVICE')
MAX_PLOTS = 4

@bokeh_app.route("/bokeh/benchmarks/")
@object_page("benchmark")
def make_benchmarks():
    app = BenchmarkApp.create()
    return app
