
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
    data_display0 = Instance(DataTable)
    data_display1 = Instance(DataTable)

    # plot and interaction
    plot = Instance(Plot)
    hover = Instance(HoverTool)
    # data
    source0 = Instance(ColumnDataSource)
    source1 = Instance(ColumnDataSource)
    source2 = Instance(ColumnDataSource)
    source3 = Instance(ColumnDataSource)

    @classmethod
    def create(cls):
        """One-time creation of app's objects.

        This function is called once, and is responsible for
        creating all objects (plots, datasources, etc)
        """
        obj = cls()

        # set up the data source
        obj.source0 = ColumnDataSource(data=dict())
        obj.source1 = ColumnDataSource(data=dict())
        obj.source2 = ColumnDataSource(data=dict())
        obj.source3 = ColumnDataSource(data=dict())

        columns = [
            TableColumn(field='x', title='x'),
            TableColumn(field='y', title='y'),
            TableColumn(field='device', title='device'),
            TableColumn(field='platform', title='platform')
        ]

#        obj.data_display0 = DataTable(source=obj.source2, columns=columns)
#        obj.data_display1 = DataTable(source=obj.source3, columns=columns)

        # setup user input
        obj.x_axis_options = Select(title="X:", value=axis_options[0], options=axis_options)
        obj.y_axis_options = Select(title="Y:", value=axis_options[2], options=axis_options)
        obj.benchmarks = Select(title="Benchmark:", value=benchmark_names[0],
            options=benchmark_names)
        obj.device_names = CheckboxGroup(labels=device_names, active=[0])

        # configure the toolset
        toolset = ['wheel_zoom,save,box_zoom,resize,reset']

        hover = HoverTool(
            tooltips = [
                ("Device", "@device"),
                ("Backend", "@platform"),
                ("(x,y)", "(@x,@y)")
            ]
        )
        toolset.append(hover)

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
        plot.line(   'x', 'y', source=obj.source0, line_width=3, line_alpha=0.6)
        plot.scatter('x', 'y', source=obj.source0, fill_color="red", size=8)

        plot.line(   'x', 'y', source=obj.source1, line_width=3, line_alpha=0.6)
        plot.scatter('x', 'y', source=obj.source1, fill_color="white", size=8)

        plot.line(   'x', 'y', source=obj.source2, line_width=3, line_alpha=0.6)
        plot.scatter('x', 'y', source=obj.source2, fill_color="blue", size=8)

        plot.line(   'x', 'y', source=obj.source3, line_width=3, line_alpha=0.6)
        plot.scatter('x', 'y', source=obj.source3, fill_color="black", size=8)

        obj.plot = plot
        obj.update_data()

        obj.inputs = VBoxForm(
            children=[obj.benchmarks, obj.device_names,
                obj.x_axis_options, obj.y_axis_options,
#                obj.data_display0, obj.data_display1
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

        # extract the data
        sources = dict()
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
            sources['x'] = x
            sources[y_id] = y
            sources[device_id] = [device] * len(x)
            sources[platform_id] = [platform] * len(x)

            # increment the counter
            result_number += 1

        # assign the data
        self.source0.data = dict()
        if 'y0' in sources:
            self.source0.data = dict(
                x = sources['x'], y = sources['y0'],
                device = sources['device0'],
                platform = sources['platform0']
            )

        self.source1.data = dict()
        if 'y1' in sources:
            self.source1.data = dict(
                x = sources['x'], y = sources['y1'],
                device = sources['device1'],
                platform = sources['platform1']
            )

        self.source2.data = dict()
        if 'y2' in sources:
            self.source2.data = dict(
                x = sources['x'], y = sources['y2'],
                device = sources['device2'],
                platform = sources['platform2']
            )

        self.source3.data = dict()
        if 'y3' in sources:
            self.source3.data = dict(
                x = sources['x'], y = sources['y3'],
                device = sources['device3'],
                platform = sources['platform3']
            )

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
