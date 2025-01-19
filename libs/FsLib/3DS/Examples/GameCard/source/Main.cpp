#include "FsLib.hpp"
#include <string>

static std::string UTF16ToUTF8(std::u16string_view Str)
{
    uint8_t Buffer[Str.length() + 1] = {0};
    utf16_to_utf8(Buffer, reinterpret_cast<const uint16_t *>(Str.data()), Str.length() + 1);
    return std::string(reinterpret_cast<const char *>(Buffer));
}

int main(void)
{
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    if (!FsLib::Initialize())
    {
        printf("%s\n", FsLib::GetErrorString());
        return -1;
    }

    // To do: Give FsLib a way to return if there's a gamecard inserted.
    if (!FsLib::OpenGameCardSaveData(u"GameCard"))
    {
        printf("%s\n", FsLib::GetErrorString());
        return -2;
    }

    // This will open and read the root directory.
    FsLib::Directory GameCardRoot(u"GameCard:/");
    if (!GameCardRoot.IsOpen())
    {
        printf("%s\n", FsLib::GetErrorString());
        return -3;
    }

    printf("GameCard Root: \n");
    for (uint32_t i = 0; i < GameCardRoot.GetEntryCount(); i++)
    {
        if (GameCardRoot.EntryAtIsDirectory(i))
        {
            printf("\tDIR %s\n", UTF16ToUTF8(GameCardRoot.GetEntryAt(i)).c_str());
        }
        else
        {
            printf("\tFIL %s\n", UTF16ToUTF8(GameCardRoot.GetEntryAt(i)).c_str());
        }
    }

    printf("Press start to exit.");

    while (aptMainLoop())
    {
        hidScanInput();

        uint32_t ButtonsDown = hidKeysDown();

        if (ButtonsDown & KEY_START)
        {
            break;
        }
    }

    FsLib::Exit();
    gfxExit();
    return 0;
}
