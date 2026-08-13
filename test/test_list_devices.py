def test_list_devices():
    from windows_capture_device_list import list_devices

    devices = list_devices()
    assert isinstance(devices, list)
    for device in devices:
        assert isinstance(device.id, int)
        assert isinstance(device.name, str)
        assert isinstance(device.resolutions, list)
        for resolution in device.resolutions:
            assert isinstance(resolution.width, int)
            assert isinstance(resolution.height, int)
