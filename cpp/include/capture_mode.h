#pragma once

#include <string>

struct CaptureDevice;

struct CaptureMode
{
    CaptureDevice *device = nullptr;
    int width = 0;
    int height = 0;
    double fps = 0.0;
    std::string format;
    std::string subtype_guid;
};
