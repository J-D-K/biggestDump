#include "biggestDump.hpp"
#include "appStates/mainState.hpp"
#include "console.hpp"
#include "fslib.hpp"
#include "input.hpp"
#include "logger.hpp"
#include "sdl.hpp"
#include "strings.hpp"
#include <switch.h>

namespace
{
    // These are the beautiful rainbow of colors BiggestDump uses.
    static constexpr sdl::Color CLEAR = {0x2D2D2DFF};
    static constexpr sdl::Color WHITE = {0xFFFFFFFF};
    static constexpr sdl::Color RED = {0xFF0000FF};
    static constexpr sdl::Color GREEN = {0x00FF00FF};
    static constexpr sdl::Color YELLOW = {0xF8FC00FF};
} // namespace

BiggestDump::BiggestDump()
{
    // Init FsLib because it's the most important thing.
    if (!fslib::initialize())
    {
        return;
    }

    // This will create a blank log to start.
    logger::initialize();

    // RomFS is here because I don't have time to write my own thing for it.
    if (R_FAILED(romfsInit()))
    {
        logger::log("Error opening romfs.");
        return;
    }

    // Shutdown fs_dev
    if (!fslib::dev::initializeSDMC())
    {
        logger::log("Error intializing fslib::dev.");
        return;
    }

    // SDL for rendering and text.
    if (!sdl::initialize("biggestDump", 1280, 720) || !sdl::text::initialize())
    {
        logger::log("Error initializing SDL and/or FreeType: %s.", sdl::getErrorString());
        return;
    }

    // Load strings from file.
    strings::initialize();

    // Game pad stuff.
    input::initialize();

    // Setup console.
    Console::setXY(56, 94);
    Console::setFontSize(22);
    Console::setMaxLineCount(19);

    // Add color key chars
    sdl::text::addColorCharacter(L'*', RED);
    sdl::text::addColorCharacter(L'<', YELLOW);
    sdl::text::addColorCharacter(L'>', GREEN);

    BiggestDump::pushState(std::make_shared<MainState>());

    // Should be good to go?
    sm_isRunning = true;
}

BiggestDump::~BiggestDump()
{
    romfsExit();
    sdl::text::exit();
    sdl::exit();
    fslib::exit();
}

bool BiggestDump::isRunning(void) const
{
    return sm_isRunning;
}

void BiggestDump::update(void)
{
    input::update();

    if (!sm_stateVector.empty())
    {
        while (!sm_stateVector.back()->isActive())
        {
            sm_stateVector.pop_back();
        }
        sm_stateVector.back()->update();
    }
}

void BiggestDump::render(void)
{
    sdl::frameBegin(CLEAR);
    // This is the very base of biggestDump's UI(?)
    sdl::renderLine(NULL, 30, 88, 1250, 88, WHITE);
    sdl::renderLine(NULL, 30, 648, 1250, 648, WHITE);
    sdl::text::render(NULL, 130, 26, 34, sdl::text::NO_TEXT_WRAP, WHITE, "biggestDump *Z*: Resurrection");
    Console::render();
    sdl::frameEnd();
}

void BiggestDump::quit(void)
{
    sm_isRunning = false;
}

void BiggestDump::pushState(std::shared_ptr<AppState> newState)
{
    sm_stateVector.push_back(newState);
}
