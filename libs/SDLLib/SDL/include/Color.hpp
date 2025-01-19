#pragma once
#include <cstdint>

// This isn't really part of SDL. Just my own way of handling this stuff.
namespace SDL
{
    union Color
    {
            // Not sure what else to name these...
            uint32_t Raw;
            // This is backwards from how you'd think it is.
            uint8_t RGBA[4];
    };
} // namespace SDL
