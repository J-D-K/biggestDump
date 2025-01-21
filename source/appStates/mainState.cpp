#include "appStates/mainState.hpp"
#include "appStates/threadState.hpp"
#include "biggestDump.hpp"
#include "console.hpp"
#include "fslib.hpp"
#include "input.hpp"
#include "logger.hpp"
#include "strings.hpp"
#include "threadFunctions.hpp"
#include "zip.hpp"
#include <switch.h>

namespace
{
    const char *FIRMWARE_FOLDER = "sdmc:/FirmwareDump";
}

MainState::MainState(void)
{
    Console::printf(strings::getByName(strings::names::WELCOME));

    // Try to mount system partition.
    if (!(m_systemMounted = fslib::openBisFileSystem("sys", FsBisPartitionId_System)))
    {
        Console::printf("*%s*: %s", fslib::getErrorString(), strings::getByName(strings::names::QUIT));
    }
    else
    {
        Console::printf(strings::getByName(strings::names::INSTRUCTIONS));
    }
}

void MainState::update(void)
{
    if (input::buttonPressed(HidNpadButton_A) && m_systemMounted)
    {
        // I don't like the following, but I guess it needs to be this way...
        // Gotta make sure this is clean first.
        if (fslib::directoryExists(FIRMWARE_FOLDER) && !fslib::deleteDirectoryRecursively(FIRMWARE_FOLDER))
        {
            Console::printf("*%s*\n", fslib::getErrorString());
            return;
        }

        if (!fslib::createDirectory(FIRMWARE_FOLDER))
        {
            return;
        }
        BiggestDump::pushState(std::make_shared<ThreadState>(thread::dumpToFolder));
    }
    else if (input::buttonPressed(HidNpadButton_X) && m_systemMounted)
    {
        // I don't think this cares about there being a previous backup.
        BiggestDump::pushState(std::make_shared<ThreadState>(thread::dumpToZip));
    }
    else if (input::buttonPressed(HidNpadButton_Plus))
    {
        BiggestDump::quit();
    }
}
