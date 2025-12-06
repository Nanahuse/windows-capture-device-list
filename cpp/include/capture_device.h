#pragma once

#include <string>
#include <vector>

#include "resolution.h"

struct CaptureDevice
{
    int id;
    std::string name;
    std::vector<Resolution> resolutions;
};
