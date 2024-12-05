#pragma once
#include "AppStates/AppState.hpp"
#include <memory>

namespace BiggestDump
{
    void Initialize(void);
    void Exit(void);
    void Quit(void);
    bool IsRunning(void);
    void Update(void);
    void Render(void);
    void PushNewState(std::shared_ptr<AppState> NewState);
} // namespace BiggestDump
