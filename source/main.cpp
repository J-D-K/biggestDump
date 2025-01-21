#include "biggestDump.hpp"
#include <switch.h>

int main(void)
{
    BiggestDump biggestDump{};
    while (appletMainLoop() && biggestDump.isRunning())
    {
        biggestDump.update();
        biggestDump.render();
    }
    return 0;
}
