# windows-capture-device-list

Windows のキャプチャデバイスと、Windows API が明示したキャプチャモードを列挙する Python ライブラリです。DirectShow がデフォルトのバックエンドで、Media Foundation も選択できます。

## DirectShow

```python
from windows_capture_device_list import list_devices

devices = list_devices()
```

## Media Foundation

```python
from windows_capture_device_list import Backend, list_devices

devices = list_devices(Backend.MEDIA_FOUNDATION)
```

## 全バックエンド

```python
devices = list_devices(None)
```

## モード選択

```python
device = devices[0]
for mode in device.modes:
    print(mode.width, mode.height, mode.fps, mode.format, mode.subtype_guid)
```

## OpenCV で開く

OpenCV は optional dependency です。デバイス列挙だけなら OpenCV は必要ありません。

```python
from windows_capture_device_list import open_video_capture

mode = device.modes[0]
cap = open_video_capture(mode)
if cap is None:
    raise RuntimeError("Failed to open capture device")
```
