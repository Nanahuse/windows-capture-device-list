"""
core module built with scikit-build-core and pybind11
"""
from __future__ import annotations
import collections.abc
import typing
__all__: list[str] = ['CaptureDevice', 'Resolution', 'list_devices', 'tmp']
class CaptureDevice:
    def __init__(self, arg0: typing.SupportsInt, arg1: str, arg2: collections.abc.Sequence[Resolution]) -> None:
        ...
    @property
    def id(self) -> int:
        ...
    @property
    def name(self) -> str:
        ...
    @property
    def resolutions(self) -> list[Resolution]:
        ...
class Resolution:
    def __init__(self, arg0: typing.SupportsInt, arg1: typing.SupportsInt) -> None:
        ...
    @property
    def height(self) -> int:
        ...
    @property
    def width(self) -> int:
        ...
def list_devices() -> list[CaptureDevice]:
    """
    List video capture devices
    """
def tmp() -> CaptureDevice:
    """
    Return a temporary CaptureDevice
    """
