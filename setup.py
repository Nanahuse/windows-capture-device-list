from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        "windows_capture_device_list.core",
        ["src/core.cpp"],
        include_dirs=[pybind11.get_include()],
        language="c++",
    )
]

setup(
    name="windows-capture-device-list",
    version="0.1.0",
    packages=["windows_capture_device_list"],
    ext_modules=ext_modules,
)
