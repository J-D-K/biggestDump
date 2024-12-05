#pragma once
#include "AppStates/AppState.hpp"
#include <functional>
#include <thread>

class ThreadState : public AppState
{
    public:
        template <typename... Args>
        ThreadState(void (*Function)(bool *, Args...), Args... Arguments)
        {
            m_Thread = std::thread(Function, &m_IsRunning, std::forward<Args>(Arguments)...);
        }

        ~ThreadState()
        {
            m_Thread.join();
        }

        void Update(void)
        {
            if (!m_IsRunning)
            {
                AppState::Deactivate();
            }
        }

    private:
        // Whether or not thread is still running.
        bool m_IsRunning = true;
        // Thread
        std::thread m_Thread;
};
