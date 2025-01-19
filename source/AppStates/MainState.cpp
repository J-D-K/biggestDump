#include "AppStates/MainState.hpp"
#include "AppStates/ThreadState.hpp"
#include "BiggestDump.hpp"
#include "Console.hpp"
#include "FsLib.hpp"
#include "Input.hpp"
#include "Logger.hpp"
#include "Strings.hpp"
#include "ThreadFunctions.hpp"
#include "Zip.hpp"

MainState::MainState(void)
{
    Console::Printf(Strings::GetByName(Strings::Names::Welcome));

    // Try to mount system partition.
    m_SystemMounted = FsLib::OpenBisFileSystem("system", FsBisPartitionId_System);
    if (!m_SystemMounted)
    {
        Console::Printf("*%s* %s.", FsLib::GetErrorString(), Strings::GetByName(Strings::Names::Quit));
    }
    else
    {
        Console::Printf(Strings::GetByName(Strings::Names::Instructions));
    }
}

void MainState::Update(void)
{
    if ((Input::ButtonPressed(HidNpadButton_A) && m_SystemMounted) && FsLib::CreateDirectory("sdmc:/FirmwareDump"))
    {
        BiggestDump::PushNewState(std::make_shared<ThreadState>(ThreadFunctions::DumpToFolder));
    }
    else if (Input::ButtonPressed(HidNpadButton_X) && m_SystemMounted)
    {
        BiggestDump::PushNewState(std::make_shared<ThreadState>(ThreadFunctions::DumpToZip));
    }
    else if (Input::ButtonPressed(HidNpadButton_Plus))
    {
        BiggestDump::Quit();
    }
}
