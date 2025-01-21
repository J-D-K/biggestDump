#pragma once
#include "appStates/appState.hpp"
#include <memory>
#include <vector>

class BiggestDump
{
    public:
        BiggestDump(void);
        ~BiggestDump();

        // Returns if biggestDump initialized successfully and is running.
        bool isRunning(void) const;
        // Runs the update routine.
        void update(void);
        // Runs the render function. Basically just renders the title and console.
        void render(void);

        // Tells biggestDump to stop running and exit. Normally I'd put this in the main update(), but I only want people to be able to quit from the main state.
        static void quit(void);
        // Pushes a new state to the state vector.
        static void pushState(std::shared_ptr<AppState> newState);

    private:
        // Whether or not biggestDump initialized everything it needs and is running.
        static inline bool sm_isRunning = false;
        // State vector
        static inline std::vector<std::shared_ptr<AppState>> sm_stateVector;
};
