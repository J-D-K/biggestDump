#include "SDL.hpp"
#include <switch.h>

static constexpr SDL::Color BLACK = {0x000000FF};
static constexpr SDL::Color WHITE = {0xFFFFFFFF};

int main(void)
{
    if (!SDL::Initialize("Test App", 1280, 720))
    {
        return -1;
    }

    if (!SDL::Text::Initialize())
    {
        return -2;
    }

    PadState Gamepad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&Gamepad);

    while (true)
    {
        padUpdate(&Gamepad);

        if (padGetButtons(&Gamepad) & HidNpadButton_Plus)
        {
            break;
        }

        SDL::FrameBegin(BLACK);
        SDL::Text::Render(NULL, 0, 0, 18, SDL::Text::NO_TEXT_WRAP, WHITE, "Text here.\nPress \uE0EF to exit.");
        SDL::FrameEnd();
    }

    SDL::Text::Exit();
    SDL::Exit();
    return 0;
}
