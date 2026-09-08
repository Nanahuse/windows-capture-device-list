from __future__ import annotations

from typing import TYPE_CHECKING

from .core import Backend, CaptureMode

if TYPE_CHECKING:
    import cv2


def _fourcc_from_guid(subtype_guid: str, cv2_module: object) -> int | None:
    parts = subtype_guid.split("-")
    if len(parts) != 5 or parts[1:] != ["0000", "0010", "8000", "00AA00389B71"]:
        return None
    try:
        data1 = int(parts[0], 16)
    except ValueError:
        return None
    fourcc = bytes((data1 >> shift) & 0xFF for shift in (0, 8, 16, 24))
    if not all(0x20 <= value <= 0x7E for value in fourcc):
        return None
    return cv2_module.VideoWriter_fourcc(*fourcc.decode("ascii"))


def open_video_capture(mode: CaptureMode) -> cv2.VideoCapture | None:
    """Open a capture mode with OpenCV.

    OpenCV is an optional dependency for this package and is imported lazily.
    It must be installed when calling this function; importing the package and
    using :func:`list_devices` do not require ``cv2``.

    Returns ``None`` when OpenCV is available but the capture cannot be opened.
    An ``ImportError`` is raised when OpenCV is not installed.
    """
    import cv2

    backend = cv2.CAP_DSHOW if mode.device.backend == Backend.DIRECT_SHOW else cv2.CAP_MSMF
    capture = cv2.VideoCapture(mode.device.index, backend)
    if mode.device.backend == Backend.DIRECT_SHOW:
        capture.set(cv2.CAP_PROP_FPS, mode.fps)
        capture.set(cv2.CAP_PROP_FRAME_WIDTH, mode.width)
        capture.set(cv2.CAP_PROP_FRAME_HEIGHT, mode.height)
        fourcc = _fourcc_from_guid(mode.subtype_guid, cv2)
        if fourcc is not None:
            capture.set(cv2.CAP_PROP_FOURCC, fourcc)
    else:
        capture.set(cv2.CAP_PROP_FRAME_WIDTH, mode.width)
        capture.set(cv2.CAP_PROP_FRAME_HEIGHT, mode.height)
        capture.set(cv2.CAP_PROP_FPS, mode.fps)
    if capture.isOpened():
        return capture
    capture.release()
    return None
