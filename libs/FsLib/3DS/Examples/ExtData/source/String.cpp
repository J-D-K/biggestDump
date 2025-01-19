#include "String.hpp"
#include <3ds.h>

std::string ToUTF8(std::u16string_view In)
{
    char UTF8Buffer[In.length() + 1] = {0};
    utf16_to_utf8(reinterpret_cast<uint8_t *>(UTF8Buffer), reinterpret_cast<const uint16_t *>(In.data()), In.length() + 1);
    return std::string(UTF8Buffer);
}
