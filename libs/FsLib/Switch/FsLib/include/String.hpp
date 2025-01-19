#pragma once
#include <string>

namespace FsLib
{
    /// @brief Contains the function for constructing error strings.
    namespace String
    {
        /// @brief This is just used internally to make setting the error string easier.
        /// @param Format Format of string.
        /// @param Arguments
        /// @return Formatted string?
        std::string GetFormattedString(const char *Format, ...);
    } // namespace String
} // namespace FsLib
