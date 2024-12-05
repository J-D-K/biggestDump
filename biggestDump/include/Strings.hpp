#pragma once
#include <string>

namespace Strings
{
    // Initializes and loads strings from respective JSON. biggestDump doesn't have many strings anyway.
    void Initialize(void);
    // Returns the string by its name. Returns NULL if the string isn't found.
    const char *GetByName(std::string_view StringName);
    // These are the names of the strings used to avoid typos and NULLs from above.
    namespace Names
    {
        static constexpr std::string_view Welcome = "Welcome";
        static constexpr std::string_view Quit = "Quit";
        static constexpr std::string_view Instructions = "Instructions";
        static constexpr std::string_view CopyingFile = "CopyingFile";
        static constexpr std::string_view CopyingFileZip = "CopyingFileZip";
        static constexpr std::string_view Done = "Done";
    } // namespace Names
} // namespace Strings
