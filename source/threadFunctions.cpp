#include "threadFunctions.hpp"
#include "console.hpp"
#include "io.hpp"
#include "strings.hpp"
#include "zip.hpp"

void thread::dumpToFolder(bool *isRunning)
{
    copyDirectory("sys:/Contents", "sdmc:/FirmwareDump");
    Console::printf(strings::getByName(strings::names::QUIT));
    *isRunning = false;
}

void thread::dumpToZip(bool *isRunning)
{
    copyDirectoryToZip("sys:/Contents", "sdmc:/FirmwareDump.zip");
    Console::printf(strings::getByName(strings::names::QUIT));
    *isRunning = false;
}
