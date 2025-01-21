#include "input.hpp"

namespace
{
    PadState s_padState;
}

void input::initialize(void)
{
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&s_padState);
}

void input::update(void)
{
    padUpdate(&s_padState);
}

bool input::buttonPressed(HidNpadButton button)
{
    return (s_padState.buttons_cur & button);
}

bool input::buttonHeld(HidNpadButton button)
{
    return (s_padState.buttons_old & button) && (s_padState.buttons_cur & button);
}

bool input::buttonReleased(HidNpadButton button)
{
    return (s_padState.buttons_old & button) && !(s_padState.buttons_cur & button);
}
