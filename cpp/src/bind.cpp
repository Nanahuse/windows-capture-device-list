#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <iterator>

#include "capture_device.h"
#include "capture_mode.h"
#include "core.h"

namespace py = pybind11;

PYBIND11_MODULE(core, m)
{
    m.doc() = "Windows Capture Device List Module";

    py::enum_<Backend>(m, "Backend")
        .value("DIRECT_SHOW", Backend::DIRECT_SHOW)
        .value("MEDIA_FOUNDATION", Backend::MEDIA_FOUNDATION);

    py::class_<CaptureMode>(m, "CaptureMode")
        .def_readonly("device", &CaptureMode::device, py::return_value_policy::reference)
        .def_readonly("width", &CaptureMode::width)
        .def_readonly("height", &CaptureMode::height)
        .def_readonly("fps", &CaptureMode::fps)
        .def_property_readonly("format", [](const CaptureMode &mode) -> py::object {
            return mode.format.empty() ? py::none() : py::cast(mode.format);
        })
        .def_readonly("subtype_guid", &CaptureMode::subtype_guid);

    py::class_<CaptureDevice>(m, "CaptureDevice")
        .def_readonly("backend", &CaptureDevice::backend)
        .def_readonly("index", &CaptureDevice::index)
        .def_readonly("name", &CaptureDevice::name)
        .def_property_readonly("modes", [](CaptureDevice &device) {
            py::list modes;
            for (const auto &mode : device.modes)
            {
                py::object value = py::cast(mode);
                value.cast<CaptureMode &>().device = &device;
                modes.append(value);
            }
            return modes;
        });

    m.def("list_devices", [](py::object backend) {
        if (backend.is_none())
        {
            auto direct_show = list_devices(Backend::DIRECT_SHOW);
            auto media_foundation = list_devices(Backend::MEDIA_FOUNDATION);
            direct_show.insert(direct_show.end(),
                               std::make_move_iterator(media_foundation.begin()),
                               std::make_move_iterator(media_foundation.end()));
            return direct_show;
        }
        return list_devices(backend.cast<Backend>());
    }, py::arg("backend") = Backend::DIRECT_SHOW, "List video capture devices");
}
