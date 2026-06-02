"""
Build: python3 setup.py build_ext --inplace

Requer no RPi5 (AGL):
    pip3 install pybind11
    OpenBLAS já incluído no AGL stack (CORTEXA76)
"""
from setuptools import setup, Extension
import pybind11
import numpy

ext = Extension(
    name="postprocess_cpp",
    sources=["postprocess.cpp"],
    include_dirs=[
        pybind11.get_include(),
        numpy.get_include(),
        "/usr/include/openblas",
    ],
    libraries=["openblas"],
    extra_compile_args=[
        "-O3",
        "-ffast-math",
        "-march=armv8-a+fp+simd",  # CORTEXA76
        "-std=c++17",
    ],
    language="c++",
)

setup(
    name="postprocess_cpp",
    version="1.0",
    ext_modules=[ext],
)
