#pragma once

class AppState
{
    public:
        AppState(void) = default;
        virtual ~AppState() {};

        virtual void Update(void) = 0;
        // Since BiggestDump uses a console, states have no render function.

        void Deactivate(void)
        {
            m_IsActive = false;
        }

        bool IsActive(void) const
        {
            return m_IsActive;
        }

    private:
        // Whether or not this state is still active or can be purged from the vector.
        bool m_IsActive = true;
};
