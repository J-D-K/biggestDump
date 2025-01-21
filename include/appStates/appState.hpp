#pragma once

class AppState
{
    public:
        AppState(void) = default;
        virtual ~AppState() {};

        virtual void update(void) = 0;
        // Since BiggestDump uses a console, states have no render function.

        void deactivate(void)
        {
            m_isActive = false;
        }

        bool isActive(void) const
        {
            return m_isActive;
        }

    private:
        // Whether or not this state is still active or can be purged from the vector.
        bool m_isActive = true;
};
