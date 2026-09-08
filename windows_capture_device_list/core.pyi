"""Windows Capture Device List extension module."""
from __future__ import annotations

from enum import Enum

__all__: list[str]


class Backend(Enum):
    DIRECT_SHOW: Backend
    MEDIA_FOUNDATION: Backend


class CaptureDevice:
    @property
    def backend(self) -> Backend: ...
    @property
    def index(self) -> int: ...
    @property
    def name(self) -> str: ...
    @property
    def modes(self) -> list[CaptureMode]: ...


class CaptureMode:
    @property
    def device(self) -> CaptureDevice: ...
    @property
    def width(self) -> int: ...
    @property
    def height(self) -> int: ...
    @property
    def fps(self) -> float: ...
    @property
    def format(self) -> str | None: ...
    @property
    def subtype_guid(self) -> str: ...


def list_devices(backend: Backend | None = Backend.DIRECT_SHOW) -> list[CaptureDevice]: ...
