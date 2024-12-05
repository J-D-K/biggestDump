#include "ThreadFunctions.hpp"
#include "IO.hpp"
#include "Zip.hpp"

void ThreadFunctions::DumpToFolder(bool *IsRunning)
{
    CopyDirectoryToDirectory("system:/Contents", "sdmc:/FirmwareDump");
    *IsRunning = false;
}

void ThreadFunctions::DumpToZip(bool *IsRunning)
{
    CopyDirectoryToZip("system:/Contents", "sdmc:/FirmwareDump.zip");
    *IsRunning = false;
}
