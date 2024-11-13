#include "Console.hpp"
#include "FsLib.hpp"
#include "IO.hpp"
#include "SDL.hpp"
#include "Zip.hpp"
#include <switch.h>

static constexpr SDL::Color CLEAR = {0x2D2D2DFF};
static constexpr SDL::Color WHITE = {0xFFFFFFFF};
static constexpr SDL::Color RED = {0xFF0000FF};
static constexpr SDL::Color GREEN = {0x00FF00FF};
static constexpr SDL::Color YELLOW = {0xF8FC00FF};

int main(void)
{
    FsLib::Initialize();

    if (!SDL::Initialize("biggestDump", 1280, 720))
    {
        return -2;
    }

    if (!SDL::Text::Initialize())
    {
        return -3;
    }

    // Setup text color symbols.
    SDL::Text::AddColorCharacter(L'*', RED);
    SDL::Text::AddColorCharacter(L'<', YELLOW);
    SDL::Text::AddColorCharacter(L'>', GREEN);

    PadState Gamepad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&Gamepad);

    Console::SetMaxLineCount(22);

    Console::Get() << ">Welcome> to *biggestDump*. The app with the most legendary icon on Switch!" << "\n";
    bool SystemOpened = FsLib::OpenBisFileSystem("system", FsBisPartitionId_System);
    if (!SystemOpened)
    {
        Console::Get() << FsLib::GetErrorString() << "\n";
    }
    else
    {
        Console::Get() << "Press \uE0E0 to dump firmware files to to *sdmc:/Firmware/* or \uE0E2 to dump them to *sdmc:/switch/Firmware.zip*."
                       << "\n";
    }

    std::thread DumpThread;
    while (true)
    {
        padUpdate(&Gamepad);

        if (SystemOpened && (padGetButtonsDown(&Gamepad) & HidNpadButton_A) && FsLib::CreateDirectory("sdmc:/FirmwareDump"))
        {
            Console::Get() << "Dumping firmware to SD... " << "\n";
            DumpThread = std::thread(CopyDirectoryToDirectory, "system:/Contents/", "sdmc:/FirmwareDump/");
        }
        else if ((padGetButtonsDown(&Gamepad) & HidNpadButton_X) && SystemOpened)
        {
            Console::Get() << "Dumping firmware to zip..." << "\n";
            DumpThread = std::thread(CopyDirectoryToZip, "system:/Contents/", "sdmc:/switch/Firmware.zip");
        }
        else if (padGetButtonsDown(&Gamepad) & HidNpadButton_Plus)
        {
            break;
        }

        SDL::FrameBegin(CLEAR);
        // Render the base.
        SDL::RenderLine(30, 88, 1250, 88, WHITE);
        SDL::RenderLine(30, 648, 1250, 648, WHITE);
        SDL::Text::Render(NULL, 130, 26, 34, SDL::Text::NO_TEXT_WRAP, WHITE, "biggestDump *Z* : Resurrection");

        Console::Render();
        SDL::FrameEnd();
    }

    SDL::Text::Exit();
    SDL::Exit();
    FsLib::Exit();
    return 0;
}
