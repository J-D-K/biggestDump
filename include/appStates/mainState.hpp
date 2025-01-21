#pragma once
#include "appStates/appState.hpp"

// This state serves no other purpose than to launch the others and control being able to quit
class MainState : public AppState
{
    public:
        MainState(void);
        ~MainState() {};

        void update(void);

    private:
        // Whether or not the system partition was mounted successfully.
        bool m_systemMounted = false;
};
