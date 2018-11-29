#include <string>
#include <fstream>
#include <vector>
#include <switch.h>
#include <sys/stat.h>

#include "gfx.h"
#include "dir.h"
#include "cons.h"

console infoCons(28);
font *sysFont;

int main(int argc, const char *argv[])
{
    graphicsInit(1280, 720);
    hidInitialize();

    sysFont = fontLoadSharedFonts();

    texClearColor(frameBuffer, clrCreateU32(0xFF2D2D2D));
    drawText("biggestDump", frameBuffer, sysFont, 64, 38, 24, clrCreateU32(0xFFFFFFFF));
    drawRect(frameBuffer, 30, 87, 1220, 1, clrCreateU32(0xFFFFFFFF));
    drawRect(frameBuffer, 30, 648, 1220, 1, clrCreateU32(0xFFFFFFFF));
    infoCons.out(sysFont, "Press A to Dump to \"sdmc:/Update/\", Plus to Exit");

    while(appletMainLoop())
    {
        hidScanInput();

        uint64_t down = hidKeysDown(CONTROLLER_P1_AUTO);

        if(down & KEY_A)
        {
            infoCons.out(sysFont, "Beginning update dump.");
            FsFileSystem sys;
            fsOpenBisFileSystem(&sys, 31, "");
            fsdevMountDevice("sys", sys);

            //del first for clean dump lol
            delDir("sdmc:/Update/");
            mkdir("sdmc:/Update", 777);

            copyDirToDir("sys:/Contents/", "sdmc:/Update/");

            fsdevUnmountDevice("sys");
            infoCons.out(sysFont, "Update dump finished. Open 'sdmc:/Update/'' in ChoiDujourNX to update!");
        }
        else if(down & KEY_PLUS)
            break;

        gfxHandleBuffs();
    }

    hidExit();
    fontDestroy(sysFont);
    graphicsExit();
}
