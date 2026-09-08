#pragma once

#include <string>
#include <vector>

#include "capture_mode.h"

enum class Backend
{
    DIRECT_SHOW,
    MEDIA_FOUNDATION,
};

struct CaptureDevice
{
    Backend backend;
    int index;
    std::string name;
    std::vector<CaptureMode> modes;
};
