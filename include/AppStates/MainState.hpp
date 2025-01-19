#pragma once
#include "AppStates/AppState.hpp"

// This state serves no other purpose than to launch the others and control being able to quit
class MainState : public AppState
{
    public:
        MainState(void);
        ~MainState() {};

        void Update(void);

    private:
        // Whether or not the system partition was mounted successfully.
        bool m_SystemMounted = false;
};
