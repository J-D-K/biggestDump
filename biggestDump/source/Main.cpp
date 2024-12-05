#include "BiggestDump.hpp"

int main(void)
{
    BiggestDump::Initialize();
    while (BiggestDump::IsRunning())
    {
        BiggestDump::Update();
        BiggestDump::Render();
    }
    BiggestDump::Exit();
    return 0;
}
