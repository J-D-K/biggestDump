#include "String.hpp"
#include <array>
#include <cstdarg>

namespace
{
    constexpr int VA_BUFFER_SIZE = 0x1000;
}

std::string SDL::String::GetFormattedString(const char *Format, ...)
{
    std::array<char, VA_BUFFER_SIZE> VaBuffer;

    std::va_list VaList;
    va_start(VaList, Format);
    vsnprintf(VaBuffer.data(), VA_BUFFER_SIZE, Format, VaList);
    va_end(VaList);

    return std::string(VaBuffer.data());
}
