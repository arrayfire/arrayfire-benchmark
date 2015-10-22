ArrayFire Benchmark Scripts
=====

## Prerequisites

1. Install bokeh, preferably via. anaconda python

## `bokeh_server.py`

This script creates a live, interactive plotting widget using the
[Bokeh](http://bokeh.pydata.org/en/latest/) plotting library.

To run the script

1. Update the `data_dir` argument in the script to point to the directory
   containing your benchmark results.
2. Run `bokeh_server --script bokeh_server.py`

## `standalone-plot.py`

This script creates static HTML files that can be embedded in iframes.
