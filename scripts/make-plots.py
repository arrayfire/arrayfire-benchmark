#!/usr/bin/python

plotter = '/usr/bin/python ./standalone-plot.py'
data_dir = '/home/bkloppenborg/workspace/arrayfire-benchmark-results/2015-10-09/all/'

class bmg:
    """Benchmark Group"""

    benchmarks = list()
    xaxis = 'size'
    yaxis = 'time'

    def __init__(self, benchmarks, xaxis, yaxis):
        self.benchmarks = ['-b ' + item for item in benchmarks]
        self.xaxis = xaxis
        self.yaxis = yaxis

class devg:
    """Device Group"""

    devices =  list()
    prefix = ""

    def __init__(self, devices, prefix=""):
        # Ensure all device names are encapsulated in quotation marks
        self.devices = ['-d "' + item + '"' for item in devices]
        self.prefix = prefix

####
# Hardware
####
device_groups = list()

server = [
    'Tesla_K20c',
    'Tesla_K40c',
    'AMD_FirePro_W8100',
    'Intel(R)_Xeon(R)_CPU'
]
device_groups.append(devg(server, 'server_'))

consumer = [
    'GeForce_GTX_780',
    'AMD_R9_280',
    'Intel(R)_Core(TM)_i7-4770K_CPU_@ 3.50GHz',
    'AMD_FX(tm)-8350_Eight-Core_Processor'
]
device_groups.append(devg(consumer, 'consumer_'))

integrated = [
    'AMD_A10-7850K_APU_with_Radeon(TM) R7 Graphics',
    'Intel(R)_HD_Graphics_4600',
    'Intel Iris'
]
device_groups.append(devg(integrated, 'integrated_'))

mobile = [
    'Tegra_K1',
    'Tegra_K1_HighPerf'
]
device_groups.append(devg(mobile, 'mobile_'))

####
# Functions
####

benchmark_groups = list()

# Linear Algebra
# Should be scaled to FLOPS
linear_algebra = [
    'MatrixMultiply_f32',
    'MatrixMultiply_f64'
]

benchmark_groups.append(bmg(linear_algebra, "size", "custom"))

# Signal processing
# should appear using absolute times
signal_proc = [
    'FFT_1D_f32',
    'FFT_1D_f64',
    'FFT_2D_f32',
    'FFT_2D_f64',
    'Convolve_f32_11x11',
    'Convolve_f32_5x5',
    'Convolve_f32_9x9',
    'Convolve_f64_11x11',
    'Convolve_f64_5x5',
    'Convolve_f64_9x9'
]
#benchmark_groups.append(bmg(signal_proc, "size", "time"))

# Image processing
# Should appear in terms of throughput
image_proc = [
    'Erode_f32_5x5',
    'Erode_f64_5x5',
    'MedianFilter_f32_4x4_PAD_SYM',
    'MedianFilter_f32_4x4_PAD_ZERO',
    'MedianFilter_f64_4x4_PAD_SYM',
    'MedianFilter_f64_4x4_PAD_ZERO',
    'Expand_2D_f32_AF_INTERP_BILINEAR',
    'Expand_2D_f32_AF_INTERP_NEAREST',
    'Expand_2D_f64_AF_INTERP_BILINEAR',
    'Expand_2D_f64_AF_INTERP_NEAREST',
    'Shrink_2D_f32_AF_INTERP_BILINEAR',
    'Shrink_2D_f32_AF_INTERP_NEAREST',
    'Shrink_2D_f64_AF_INTERP_BILINEAR',
    'Shrink_2D_f64_AF_INTERP_NEAREST',
    'Histogram_f32',
    'Histogram_f64',
    'BilateralFilter_f32',
    'BilateralFilter_f64'
]
benchmark_groups.append(bmg(image_proc, "size", "throughput"))

# vector algorithims
vector_proc = [
    'Sum_1D_f32',
    'Sum_1D_f64',
    'Sum_2D_f32',
    'Sum_2D_f64',
    'Accumulate_1D_f32',
    'Accumulate_1D_f64',
    'Accumulate_2D_f32',
    'Accumulate_2D_f64'
]
#benchmark_groups.append(bmg(vector_proc, "size", "custom"))

# computer vision
computer_vision = [
    'Image_FAST',
    'Image_ORB'
]
benchmark_groups.append(bmg(computer_vision, "size", "throughput"))



for d_group in device_groups:

    devices = d_group.devices
    prefix = d_group.prefix

    for b_group in benchmark_groups:

        cmd = [plotter, data_dir,
            "--xaxis " + b_group.xaxis,
            "--yaxis " + b_group.yaxis,
            "--save-prefix " + prefix]
        cmd.extend(b_group.benchmarks)
        cmd.extend(devices)

        cmd = ' '.join(cmd)

        print cmd

