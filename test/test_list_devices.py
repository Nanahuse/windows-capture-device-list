import re

import pytest

from windows_capture_device_list import Backend, CaptureDevice, CaptureMode, list_devices


def assert_device_shape(devices):
    for device in devices:
        assert isinstance(device.index, int)
        assert isinstance(device.name, str)
        assert isinstance(device.backend, Backend)
        assert isinstance(device.modes, list)
        seen = set()
        for mode in device.modes:
            assert isinstance(mode, CaptureMode)
            assert mode.device.backend == device.backend
            assert isinstance(mode.width, int)
            assert isinstance(mode.height, int)
            assert isinstance(mode.fps, float)
            assert mode.format is None or isinstance(mode.format, str)
            assert re.fullmatch(r"[0-9A-F]{8}-[0-9A-F]{4}-[0-9A-F]{4}-[0-9A-F]{4}-[0-9A-F]{12}", mode.subtype_guid)
            key = (mode.width, mode.height, mode.fps, mode.subtype_guid)
            assert key not in seen
            seen.add(key)


@pytest.mark.parametrize("backend", [None, Backend.DIRECT_SHOW, Backend.MEDIA_FOUNDATION])
def test_list_devices_shape(backend):
    assert_device_shape(list_devices(backend))


def test_backend_is_public():
    assert Backend.DIRECT_SHOW is not Backend.MEDIA_FOUNDATION


def test_devices_have_readable_repr():
    devices = list_devices()
    if devices:
        text = repr(devices[0])
        assert "CaptureDevice(" in text
        assert devices[0].name in text
        if devices[0].modes:
            assert "CaptureMode(" in repr(devices[0].modes[0])
