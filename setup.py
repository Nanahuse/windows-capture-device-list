from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        "windows_capture_device_list.core",
        [
            "cpp/src/core.cpp",
            "cpp/src/bind.cpp",
        ],
        include_dirs=[pybind11.get_include(), "cpp/include"],
        extra_compile_args=["-std=c++17"],
        language="c++",
    )
]

setup(
    packages=["windows_capture_device_list"],
    ext_modules=ext_modules,
)
