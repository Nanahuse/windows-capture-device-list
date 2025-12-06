#pragma once

#include <vector>

#include "capture_device.h"

/**
 * @brief List all available video capture devices on the system.
 *
 * @return std::vector<CaptureDevice> A vector containing information about each capture device.
 */
std::vector<CaptureDevice> list_devices();
