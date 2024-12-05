#include "ThreadFunctions.hpp"
#include "Console.hpp"
#include "IO.hpp"
#include "Strings.hpp"
#include "Zip.hpp"

void ThreadFunctions::DumpToFolder(bool *IsRunning)
{
    CopyDirectoryToDirectory("system:/Contents", "sdmc:/FirmwareDump");
    Console::Printf(Strings::GetByName(Strings::Names::Quit));
    *IsRunning = false;
}

void ThreadFunctions::DumpToZip(bool *IsRunning)
{
    CopyDirectoryToZip("system:/Contents", "sdmc:/FirmwareDump.zip");
    Console::Printf(Strings::GetByName(Strings::Names::Quit));
    *IsRunning = false;
}
