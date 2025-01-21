#pragma once
#include "appStates/appState.hpp"
#include <functional>
#include <switch.h>
#include <thread>

// All task functions in biggestDump must take an input pointer to a bool to let this state know when it's finished.
using threadFunction = void (*)(bool *);

class ThreadState : public AppState
{
    public:
        // F
        ThreadState(threadFunction function)
        {
            // Block home menu
            appletBeginBlockingHomeButton(0);
            // Spawn thread.
            m_thread = std::thread(function, &m_isRunning);
        }

        ~ThreadState()
        {
            // End thread.
            m_thread.join();
            appletEndBlockingHomeButton();
        }

        void update(void)
        {
            if (!m_isRunning)
            {
                AppState::deactivate();
            }
        }

    private:
        // Whether or not thread is still running.
        bool m_isRunning = true;
        // Thread
        std::thread m_thread;
};
