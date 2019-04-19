#include <string>
#include <fstream>
#include <vector>
#include <switch.h>
#include <sys/stat.h>

#include "gfx.h"
#include "dir.h"
#include "cons.h"
#include "dump.h"

console infoCons(28);
font *sysFont;

int main(int argc, const char *argv[])
{
    graphicsInit(1280, 720);
    hidInitialize();

    sysFont = fontLoadSharedFonts();

    //top
    tex *top = texCreate(1280, 88);
    //bottom
    tex *bot = texCreate(1280, 72);

    //Top
    texClearColor(top, clrCreateU32(0xFF2D2D2D));
    drawText("biggestDump", top, sysFont, 64, 38, 24, clrCreateU32(0xFFFFFFFF));
    drawRect(top, 30, 87, 1220, 1, clrCreateU32(0xFFFFFFFF));

    //Bot
    texClearColor(bot, clrCreateU32(0xFF2D2D2D));
    drawRect(bot, 30, 0, 1220, 1, clrCreateU32(0xFFFFFFFF));
    infoCons.out("Press A to Dump to #sdmc:/Update/#, X to Erase pending update, Plus to Exit");
    infoCons.nl();

    //Thread stuff so UI can update right with new GFX
    Thread workThread;
    dumpArgs *da;
    bool threadRunning = false, threadFin;
    while(appletMainLoop())
    {
        hidScanInput();

        uint64_t down = hidKeysDown(CONTROLLER_P1_AUTO);

        if(!threadRunning)
        {
            if(down & KEY_A)
            {
                threadRunning = true;
                threadFin = false;

                //Struct to send and receive stuff from thread
                da = dumpArgsCreate(&infoCons, &threadFin);
                threadCreate(&workThread, dumpThread, da, 0x4000, 0x2B, -2);
                threadStart(&workThread);
            }
            else if(down & KEY_X)
            {
                threadRunning = true;
                threadFin = false;
                da = dumpArgsCreate(&infoCons, &threadFin);
                threadCreate(&workThread, delThread, da, 0x4000, 0x2B, -2);
                threadStart(&workThread);
            }
            else if(down & KEY_PLUS)
                break;
        }
        else
        {
            if(threadFin)
            {
                threadClose(&workThread);
                dumpArgsDestroy(da);
                threadRunning = false;
            }
        }

        gfxBeginFrame();
        texClearColor(frameBuffer, clrCreateU32(0xFF2D2D2D));
        texDrawNoAlpha(top, frameBuffer, 0, 0);
        texDrawNoAlpha(bot, frameBuffer, 0, 648);
        infoCons.draw(sysFont);
        gfxEndFrame();
    }

    hidExit();
    fontDestroy(sysFont);
    texDestroy(top);
    texDestroy(bot);
    graphicsExit();
}
