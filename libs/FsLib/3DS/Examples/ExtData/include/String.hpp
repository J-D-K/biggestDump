#pragma once
#include <string>

// Takes a UTF16 string and returns a standard C++ string encoded in UTF-8 for printing and stuff.
std::string ToUTF8(std::u16string_view In);
