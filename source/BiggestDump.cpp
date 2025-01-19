#include "BiggestDump.hpp"
#include "AppStates/MainState.hpp"
#include "Console.hpp"
#include "FsLib.hpp"
#include "Input.hpp"
#include "Logger.hpp"
#include "SDL.hpp"
#include "Strings.hpp"
#include <vector>

namespace
{
    // This is whether or not BiggestDump started correctly and is running.
    bool s_IsRunning = false;
    // This is the vector of states.
    std::vector<std::shared_ptr<AppState>> s_StateVector;
    // These are the beautiful rainbow of colors BiggestDump uses.
    static constexpr SDL::Color CLEAR = {0x2D2D2DFF};
    static constexpr SDL::Color WHITE = {0xFFFFFFFF};
    static constexpr SDL::Color RED = {0xFF0000FF};
    static constexpr SDL::Color GREEN = {0x00FF00FF};
    static constexpr SDL::Color YELLOW = {0xF8FC00FF};
} // namespace

void BiggestDump::Initialize(void)
{
    // Init FsLib and SDLLib
    if (!FsLib::Initialize())
    {
        return;
    }

    // This will just kill fs_dev. We don't need two SD handles.
    // FsLib::Dev::InitializeSDMC();

    Logger::Initialize();

    if (!SDL::Initialize("biggestDump", 1280, 720))
    {
        Logger::Log("Error initializing SDL: %s", SDL::GetErrorString());
        return;
    }

    if (!SDL::Text::Initialize())
    {
        Logger::Log("Error initializing SDL::Text: %s", SDL::GetErrorString());
        return;
    }

    Input::Initialize();

    if (R_FAILED(romfsInit()))
    {
        Logger::Log("Error opening RomFS.");
        return;
    }

    Strings::Initialize();

    // Set the maximum amount of lines the console should print
    Console::SetMaxLineCount(21);

    // Add these characters to the map change colors when they're encountered.
    SDL::Text::AddColorCharacter(L'*', RED);
    SDL::Text::AddColorCharacter(L'<', YELLOW);
    SDL::Text::AddColorCharacter(L'>', GREEN);

    BiggestDump::PushNewState(std::make_shared<MainState>());

    s_IsRunning = true;
}

void BiggestDump::Exit(void)
{
    romfsExit();
    SDL::Text::Exit();
    SDL::Exit();
    FsLib::Exit();
}

void BiggestDump::Quit(void)
{
    s_IsRunning = false;
}

bool BiggestDump::IsRunning(void)
{
    return s_IsRunning;
}

void BiggestDump::Update(void)
{
    Input::Update();

    if (!s_StateVector.empty())
    {
        while (!s_StateVector.back()->IsActive())
        {
            s_StateVector.pop_back();
        }
        s_StateVector.back()->Update();
    }
}

void BiggestDump::Render(void)
{
    SDL::FrameBegin(CLEAR);
    // This is the very base of BiggestDump
    SDL::RenderLine(NULL, 30, 88, 1250, 88, WHITE);
    SDL::RenderLine(NULL, 30, 648, 1250, 648, WHITE);
    SDL::Text::Render(NULL, 130, 26, 34, SDL::Text::NO_TEXT_WRAP, WHITE, "biggestDump *Z* : Resurrection");
    Console::Render();
    SDL::FrameEnd();
}

void BiggestDump::PushNewState(std::shared_ptr<AppState> NewState)
{
    s_StateVector.push_back(NewState);
}
