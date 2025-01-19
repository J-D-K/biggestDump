#include "Input.hpp"

namespace
{
    PadState s_PadState;
}

void Input::Initialize(void)
{
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&s_PadState);
}

void Input::Update(void)
{
    padUpdate(&s_PadState);
}

bool Input::ButtonPressed(HidNpadButton Button)
{
    return (s_PadState.buttons_cur & Button);
}

bool Input::ButtonHeld(HidNpadButton Button)
{
    return (s_PadState.buttons_old & Button) && (s_PadState.buttons_cur & Button);
}

bool Input::ButtonReleased(HidNpadButton Button)
{
    return s_PadState.buttons_old & Button && !(s_PadState.buttons_cur & Button);
}
