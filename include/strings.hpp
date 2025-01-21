#pragma once
#include <string>

namespace strings
{
    // Initializes and loads strings from respective JSON. biggestDump doesn't have many strings anyway.
    void initialize(void);
    // Returns the string by its name. Returns NULL if the string isn't found.
    const char *getByName(std::string_view stringName);
    // These are the names of the strings used to avoid typos and NULLs from above.
    namespace names
    {
        // To do: Figure out exactly how to name these. They're constant, so normally all caps. Dunno yet.
        static constexpr std::string_view WELCOME = "Welcome";
        static constexpr std::string_view QUIT = "Quit";
        static constexpr std::string_view INSTRUCTIONS = "Instructions";
        static constexpr std::string_view COPYING_FILE = "CopyingFile";
        static constexpr std::string_view COPYING_FILE_ZIP = "CopyingFileZip";
        static constexpr std::string_view DONE = "Done";
    } // namespace names
} // namespace strings
