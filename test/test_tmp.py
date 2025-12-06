def test_tmp():
    from windows_capture_device_list import tmp

    assert tmp().id == 1
    assert tmp().resolution.width == 1920
    assert tmp().resolution.height == 1080
    assert tmp().name == "Camera 1"


def test_list_devices():
    from windows_capture_device_list import list_devices

    devices = list_devices()
    assert len(devices) >= 1
    for device in devices:
        assert isinstance(device.id, int)
        assert isinstance(device.name, str)
        assert isinstance(device.resolution.width, int)
        assert isinstance(device.resolution.height, int)
