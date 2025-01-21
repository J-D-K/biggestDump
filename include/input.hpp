#pragma once
#include <switch.h>

namespace input
{
    // Inits PadState
    void initialize(void);
    // Updates PadState
    void update(void);
    // Returns if button has been [X]'d
    bool buttonPressed(HidNpadButton button);
    bool buttonHeld(HidNpadButton button);
    bool buttonReleased(HidNpadButton button);
} // namespace input
