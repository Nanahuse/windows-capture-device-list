#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "capture_device.h"
#include "resolution.h"
#include "core.h"

namespace py = pybind11;

PYBIND11_MODULE(core, m)
{
    m.doc() = "Windows Capture Device List Module";

    py::class_<Resolution>(m, "Resolution")
        .def(py::init<int, int>())
        .def_readonly("width", &Resolution::width)
        .def_readonly("height", &Resolution::height);

    py::class_<CaptureDevice>(m, "CaptureDevice")
        .def(py::init<int, std::string, std::vector<Resolution>>())
        .def_readonly("id", &CaptureDevice::id)
        .def_readonly("name", &CaptureDevice::name)
        .def_readonly("resolutions", &CaptureDevice::resolutions);

    m.def("list_devices", &list_devices, "List video capture devices");
}
