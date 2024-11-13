#include "Console.hpp"
#include "FsLib.hpp"
#include "SDL.hpp"
#include <chrono>
#include <switch.h>
#include <thread>

static constexpr SDL::Color CLEAR = {0x2D2D2DFF};
static constexpr SDL::Color WHITE = {0xFFFFFFFF};
static constexpr SDL::Color RED = {0xFF0000FF};
static constexpr SDL::Color GREEN = {0x00FF00FF};
static constexpr SDL::Color YELLOW = {0xF8FC00FF};

// This is threaded just to be sure everything inside Console works without deadlocks.
static void PrintDirectory(FsLib::Path DirectoryPath)
{
    FsLib::Directory Dir(DirectoryPath);
    if (!Dir.IsOpen())
    {
        return;
    }

    for (int64_t i = 0; i < Dir.GetEntryCount(); i++)
    {
        const char *EntryName = Dir.GetEntryAt(i).data();
        if (Dir.EntryAtIsDirectory(i))
        {
            FsLib::Path NewPath = DirectoryPath + EntryName + "/";
            PrintDirectory(NewPath);
        }
        else
        {
            Console::Get() << DirectoryPath.GetFullPath() << EntryName << "\n";
            //std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
}

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
        Console::Get() << "Press \uE0E0 to list the files on your system partition until I finish biggestDump tomorrow. It's actually really "
                          "fast. Don't blink."
                       << "\n";
    }

    // To do tomorrow: Get this threading system sorted. I don't like this as it is.
    std::thread PrintThread;
    bool ThreadSpawned = false, ThreadRunning = false;
    while (true)
    {
        padUpdate(&Gamepad);

        if ((padGetButtonsDown(&Gamepad) & HidNpadButton_A) && SystemOpened)
        {
            PrintThread = std::thread(PrintDirectory, "system:/");
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
