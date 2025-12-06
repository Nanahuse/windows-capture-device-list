#pragma once

struct Resolution
{
    int width;
    int height;

    bool operator==(const Resolution &other) const
    {
        return width == other.width && height == other.height;
    }
};