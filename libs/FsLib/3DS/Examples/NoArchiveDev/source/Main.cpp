#include "FsLib.hpp"
#include <3ds.h>

extern "C"
{
    void __appInit(void)
    {
        srvInit();
        aptInit();
        hidInit();
        fsInit();
    }

    void __appExit(void)
    {
        fsExit();
        hidExit();
        aptExit();
        srvExit();
    }
}

// To do: Finish this.
int main(int argc, const char *arg[])
{
    if (!FsLib::Initialize())
    {
        return -1;
    }

    if (!FsLib::Dev::InitializeSDMC())
    {
        return -2;
    }

    /*
        This should work now, but only on SD. You should be using FsLib::File whenever possible. This is basically just a wrapper
        around a wrapper here...
    */
    FILE *TestFile = fopen("sdmc:/NoArchiveDev.txt", "w");
    fputs("Glad we got rid of that problem. :^)", TestFile);
    fclose(TestFile);

    FsLib::Exit();
}
