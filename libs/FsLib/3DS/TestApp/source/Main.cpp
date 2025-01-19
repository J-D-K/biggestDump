#include "FsLib.hpp"
#include <3ds.h>
#include <fstream>
#include <minizip/unzip.h>
#include <minizip/zip.h>
#include <string>

static std::u16string_view REALLY_LONG_DIR_PATH =
    u"sdmc:/A/Really/Long/Chain/Of/Folders/To/Make/Sure/Create/Directories/Recursively/Works/Right/And/Doesnt/Crash/The/3DS";

const char *ZipString = "asdsf;kjsaf;kjasd;fkjas;dfkja'sdfj'asldjf'asljdf'askjldf'asjdf'asjkdf'kasjdf'kajsdf'kajsdf'kajsdf'askdjf'askdjf'";

static std::string UTF16ToUTF8(std::u16string_view Str)
{
    // This isn't the best idea.
    uint8_t UTF8Buffer[Str.length() + 1] = {0};
    utf16_to_utf8(UTF8Buffer, reinterpret_cast<const uint16_t *>(Str.data()), Str.length() + 1);
    return std::string(reinterpret_cast<const char *>(UTF8Buffer));
}

// This will completely cut out archive_dev.
extern "C"
{
    u32 __stacksize__ = 0x20000;
    void __appInit(void)
    {
        srvInit();
        aptInit();
        fsInit();
        hidInit();
    }

    void __appExit(void)
    {
        hidExit();
        fsExit();
        aptExit();
        srvExit();
    }
}

int main(void)
{
    hidInit();
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    printf("FsLib Test App\n");

    if (!FsLib::Initialize())
    {
        printf("%s\n", FsLib::GetErrorString());
        return -1;
    }

    // This will initialize the barebones newlib->fslib layer.
    if (!FsLib::Dev::InitializeSDMC())
    {
        printf("FsLib::Dev Failed.\n");
        return -2;
    }


    FsLib::Path JKSMRoot = u"sdmc:/JKSM";
    if (!FsLib::DeleteDirectoryRecursively(JKSMRoot))
    {
        printf("%s\n", FsLib::GetErrorString());
    }

    printf("Press Start to exit.");
    while (aptMainLoop())
    {
        hidScanInput();
        if (hidKeysDown() & KEY_START)
        {
            break;
        }
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    FsLib::Exit();
    gfxExit();
    hidExit();
    return 0;
}
