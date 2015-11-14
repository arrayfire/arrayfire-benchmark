#!/usr/bin/python

from subprocess import call
from os import system

plotter = '/home/bkloppenborg/anaconda/bin/python /home/bkloppenborg/workspace/arrayfire-benchmark/scripts/standalone-plot.py'
data_dir = '/home/bkloppenborg/workspace/arrayfire-benchmark-results/2015-10-09/all/'

class bmg:
    """Benchmark Group"""

    benchmarks = list()
    xaxis = 'size'
    yaxis = 'time'
    options = list()

    def __init__(self, benchmarks, xaxis, yaxis, options=list()):
        self.benchmarks = ['-b ' + item for item in benchmarks]
        self.xaxis = xaxis
        self.yaxis = yaxis
        self.options = options

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

cpu = [
    'Intel(R)_Xeon(R)_CPU',
    'Intel(R)_Core(TM)_i7-4770K_CPU_@ 3.50GHz',
    'AMD_FX(tm)-8350_Eight-Core_Processor',
    'AMD_A10-7850K_APU_with_Radeon(TM) R7 Graphics  '
]
#device_groups.append(devg(cpu, 'cpu_'))

gpu = [
#    'Tesla_K20c',
    'Tesla_K40c',
    'AMD_FirePro_W8100',
#    'GeForce_GTX_780',
#    'AMD_R9_280'
]
device_groups.append(devg(gpu, 'gpu_'))

integrated = [
    'AMD R7 Graphics Spectre',
    'AMD_A10-7850K_APU_with_Radeon(TM) R7 Graphics',
    'Intel(R)_HD_Graphics_4600',
    'Intel(R)_Core(TM)_i7-4770K_CPU_@ 3.50GHz',
    'Intel Iris'
]
#device_groups.append(devg(integrated, 'integrated_'))

mobile = [
    'Tegra_K1',
    'Tegra_K1_HighPerf',
    'Jetson-TX1'
]
#device_groups.append(devg(mobile, 'mobile_'))

####
# Functions
####

benchmark_groups = list()

# Linear Algebra
# Should be scaled to FLOPS
#matrix_multiply = [
#    'MatrixMultiply_f32',
#    'MatrixMultiply_f64'
#]
#benchmark_groups.append(bmg(matrix_multiply, "sqrt-size", "matmul-flops"))
#
## Reduce, yaxis in bandwidth
#benchmark_groups.append(bmg(['Sum_1D_f32', 'Sum_2D_f32'], 'size', 'bandwidth-r1-w0-f32'))
#benchmark_groups.append(bmg(['Sum_1D_f64', 'Sum_2D_f64'], 'size', 'bandwidth-r1-w0-f64'))

# SAXPY, yaxis in bandwidth(
benchmark_groups.append(bmg(['JIT_f32_AXPY', 'NOJIT_f32_AXPY'], 'size', 'bandwidth-r2-w0-f32',
   options=['--merge-plots', '--os Windows']))
benchmark_groups.append(bmg(['JIT_f64_AXPY', 'NOJIT_f64_AXPY'], 'size', 'bandwidth-r2-w0-f64',
   options=['--merge-plots', '--os Windows']))

## Scan, yaxis in bandwidth
#benchmark_groups.append(bmg(['Accumulate_1D_f32'], 'size', 'bandwidth-r1-w0-f32'))
#benchmark_groups.append(bmg(['Accumulate_1D_f64'], 'size', 'bandwidth-r1-w0-f64'))
#
## Signal processing
## should appear using absolute times
#signal_proc = [
#    'FFT_1D_f32',
#    'FFT_1D_f64',
#    'FFT_2D_f32',
#    'FFT_2D_f64',
#    'Convolve_f32_11x11',
#    'Convolve_f32_5x5',
#    'Convolve_f32_9x9',
#    'Convolve_f64_11x11',
#    'Convolve_f64_5x5',
#    'Convolve_f64_9x9'
#]
#benchmark_groups.append(bmg(signal_proc, "size", "time"))
#
## Image processing
## Should appear in terms of throughput
#image_proc = [
#    'Erode_f32_5x5',
#    'Erode_f64_5x5',
#    'MedianFilter_f32_4x4_PAD_SYM',
#    'MedianFilter_f32_4x4_PAD_ZERO',
#    'MedianFilter_f64_4x4_PAD_SYM',
#    'MedianFilter_f64_4x4_PAD_ZERO',
#    'Expand_2D_f32_AF_INTERP_BILINEAR',
#    'Expand_2D_f32_AF_INTERP_NEAREST',
#    'Expand_2D_f64_AF_INTERP_BILINEAR',
#    'Expand_2D_f64_AF_INTERP_NEAREST',
#    'Shrink_2D_f32_AF_INTERP_BILINEAR',
#    'Shrink_2D_f32_AF_INTERP_NEAREST',
#    'Shrink_2D_f64_AF_INTERP_BILINEAR',
#    'Shrink_2D_f64_AF_INTERP_NEAREST',
#    'Histogram_f32',
#    'Histogram_f64',
#    'Image_Histogram',
#    'BilateralFilter_f32',
#    'BilateralFilter_f64'
#]
#benchmark_groups.append(bmg(image_proc, "size", "throughput"))
#
## computer vision
#computer_vision = [
#    'Image_FAST',
#    'Image_ORB'
#]
#benchmark_groups.append(bmg(computer_vision, "size", "throughput"))

# generate the plots
for d_group in device_groups:

    devices = d_group.devices
    prefix = d_group.prefix

    for b_group in benchmark_groups:

        cmd = [plotter, data_dir,
            "--xaxis " + b_group.xaxis,
            "--yaxis " + b_group.yaxis,
            "--save-prefix " + prefix]
        if len(b_group.options) > 0:
            cmd.extend(b_group.options)

        cmd.extend(b_group.benchmarks)
        cmd.extend(devices)

#        print "Now running..."
#        print ' ' + ' '.join(cmd)
        system(' '.join(cmd))

