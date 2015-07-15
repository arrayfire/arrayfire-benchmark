
# bokeh includes/configuration
import logging
logging.basicConfig(level=logging.DEBUG)
from bokeh.plotting import figure, Figure
from bokeh.models import Plot, ColumnDataSource, HoverTool
from bokeh.properties import Instance
from bokeh.server.app import bokeh_app
from bokeh.server.utils.plugins import object_page
from bokeh.models.widgets import HBox, Select, VBoxForm, MultiSelect

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

    # user input
    benchmarks = Instance(Select)
    x_axis_options = Instance(Select)
    y_axis_options = Instance(Select)
    device_names = Instance(MultiSelect)

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

        obj.source = ColumnDataSource(data=dict(x=[], y=[]))

        # setup user input
        obj.x_axis_options = Select(title="X:", value=axis_options[0], options=axis_options)
        obj.y_axis_options = Select(title="Y:", value=axis_options[2], options=axis_options)
        obj.benchmarks = Select(title="Benchmark:", value=benchmark_names[0],
            options=benchmark_names)
        obj.device_names = MultiSelect(title="Devices (bokeh display bug here):", value=[device_names[0]], options=device_names)

        # configure the hover box
        obj.hover = HoverTool(
            tooltips = [
                ("Device", "@device"),
                ("Backend", "@platform"),
                ("(x,y)", "(@x,@y)")
            ])

        toolset = [obj.hover,'save,box_zoom,resize,reset']

        title = obj.benchmarks.value + " " + \
            "(" + obj.y_axis_options.value + " vs." + obj.x_axis_options.value + ")"


        # Generate a figure container
        plot = figure(title_text_font_size="12pt",
                      plot_height=400,
                      plot_width=400,
                      tools=toolset,
                      title=title,
        )

        # Plot the line by the x,y values in the source property
        plot.line('x', 'y', source=obj.source,
            line_width=3,
            line_alpha=0.6,
        )
        plot.scatter('x', 'y', source=obj.source,
            fill_color="white",
            size=8,
        )

        obj.plot = plot
        obj.update_data()

        obj.inputs = VBoxForm(
            children=[obj.benchmarks, obj.device_names,
                obj.x_axis_options, obj.y_axis_options,
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

        # Text box event registration
        self.benchmarks.on_change('value', self, 'input_change')
        self.x_axis_options.on_change('value', self, 'input_change')
        self.y_axis_options.on_change('value', self, 'input_change')

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

    def update_data(self):
        """Called each time that any watched property changes.

        This updates the sin wave data with the most recent values of the
        sliders. This is stored as two numpy arrays in a dict into the app's
        data source property.
        """

        # apply filters based upon the user's selection
        benchmark = self.benchmarks.value
        devices = self.device_names.value

        # extract only the results which match this group
        filtered_results = filter(lambda x: x['benchmark_name'] == benchmark, celero_results)
        # remove the baseline measurements from the plots
        filtered_results = filter(lambda x: x['benchmark_name'] != "Baseline", filtered_results)

        # select the desired devices
        filtered_results = filter(lambda x: x['extra_data']['AF_DEVICE'] in devices, filtered_results)

        # Temporary, grab the first result
        result = filtered_results[0]

        # Extract the results from the benchmark
        platform = result['extra_data']['AF_PLATFORM']
        device = result['extra_data']['AF_DEVICE']

        x = self.getXY(result, self.x_axis_options.value)
        y = self.getXY(result, self.y_axis_options.value)

        print benchmark
        print devices

        logging.debug("Params: %s %s", benchmark, devices)

        # store the benchmark results in the self.source object
        # NOTE: we replicate the device and platform data here so that
        # it works correctly with the mouseover/hover
        self.source.data = dict(x=x, y=y,
            device=[device]*len(x),
            platform=[platform]*len(x))

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

@bokeh_app.route("/bokeh/benchmarks/")
@object_page("benchmark")
def make_benchmarks():
    app = BenchmarkApp.create()
    return app
