#include "FsLib.hpp"
#include "IO.hpp"
#include <memory>

// You can use this, but might not want to because if something else uses the same naming for anything it will cause issues.
// using namespace FsLib;

/*
    This is only the NA version of the game. You're going to need to handle the versions from other regions too.
    Different regions generally use separate, different archive ids.
*/
static constexpr uint64_t TITLE_ID = 0x00040000001B8700;

// This is the target folder on SD to backup and restore from.
static constexpr std::u16string_view TARGET_DIRECTORY = u"sdmc:/MinecraftSave";

int main(void)
{
    // This is standard ctrulib stuff.
    gfxInitDefault();
    if (!consoleInit(GFX_TOP, NULL))
    {
        return -1;
    }

    // This makes sure fsInit is called and maps the sd card to "sdmc".
    if (!FsLib::Initialize())
    {
        /*
            This will print the internal error string to the screen for a split second before the program returns and closes.
            Can't really log the error to a file if the lib didn't start correctly... If you want, you can call fsInit() and use C
            stdio or std::ofstream for logging errors to be sure of what is happening, but this shouldn't fail ever.
        */
        printf("%s\n", FsLib::GetErrorString());
        return -2;
    }

    /*
        This will open and map the Archive to u"ExtData:/" if it succeeds.
        It's written like this so you have an idea of how to handle this with full title ids, but using the truncated, shortened one will work
        fine for this.
    */
    if (!FsLib::OpenExtData(u"ExtData", static_cast<uint32_t>(TITLE_ID >> 8 & 0x00FFFFFF)))
    {
        printf("%s\n", FsLib::GetErrorString());
        return -3;
    }

    printf("Press A to backup ExtData Normally\nPress Y to backup ExtData Accurately\nPress X to copy SD card to ExtData\nStart to exit.\n");
    while (aptMainLoop())
    {
        hidScanInput();

        /*
            This is only supposed to return if buttons are pressed for the current frame, but it seems broken now and has issues.
            Holding the buttons will make this example repeat operations which shouldn't happen.
        */
        uint32_t ButtonsDown = hidKeysDown();

        if (ButtonsDown & KEY_A)
        {
            printf("Backing up save data to sdmc:/MinecraftSave/... \n");
            /*
                The 3DS accepts two different path types: ASCII and UTF-16. Most of 3DS and ctrulib's internal FS structs use UTF-16 so that's
                why I went with that over ASCII. The path class of FsLib will automatically handle assignment and appending to and from the
                following types:
                    FsLib::Path
                    const char16_t* and const uint16_t*. UTF-16 C strings.
                    const std::u16string and std::u16string_view. UTF-16 C++ strings.

                Typing u before a string will indicate it's unicode and the path class will take care of the rest even for functions which is
                how the following works.
            */
            // This if statement will check for and try to create the target folder if it doesn't exist.
            if (!FsLib::DirectoryExists(TARGET_DIRECTORY) && !FsLib::CreateDirectory(TARGET_DIRECTORY))
            {
                printf("%s\n", FsLib::GetErrorString());
                continue;
            }

            // This will recursively copy the ExtData archive/device to the folder on SD we just created.
            CopyDirectoryTo(u"ExtData:/", TARGET_DIRECTORY);
            printf("Done!\n");
        }
        else if (ButtonsDown & KEY_X)
        {
            // Check if the directory on SD exists. If it doesn't, just continue the loop.
            if (!FsLib::DirectoryExists(u"sdmc:/MinecraftSave"))
            {
                continue;
            }

            printf("Importing Data from SD card... \n");
            CopyDirectoryTo(u"sdmc:/MinecraftSave", u"ExtData:/");
            printf("Done!\n");
        }
        else if (ButtonsDown & KEY_START)
        {
            // Just break the loop.
            break;
        }
    }

    FsLib::Exit();
    gfxExit();
    return 0;
}
