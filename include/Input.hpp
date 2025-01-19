#pragma once
#include <switch.h>

namespace Input
{
    // Inits PadState
    void Initialize(void);
    // Updates PadState
    void Update(void);
    // Returns if button has been [X]'d
    bool ButtonPressed(HidNpadButton Button);
    bool ButtonHeld(HidNpadButton Button);
    bool ButtonReleased(HidNpadButton Button);
} // namespace Input
