#include "Logger.hpp"
#include "FsLib.hpp"
#include <cstdarg>

namespace
{
    constexpr size_t VA_BUFFER_SIZE = 0x1000;
    const char *s_LogPath = "sdmc:/switch/biggestDump.log";
} // namespace

void Logger::Initialize(void)
{
    FsLib::File LogFile(s_LogPath, FsOpenMode_Create);
}

void Logger::Log(const char *Format, ...)
{
    char VaBuffer[VA_BUFFER_SIZE];
    std::va_list VaList;
    va_start(VaList, Format);
    vsnprintf(VaBuffer, VA_BUFFER_SIZE, Format, VaList);
    va_end(VaList);

    FsLib::File LogFile(s_LogPath, FsOpenMode_Append);
    LogFile << VaBuffer << "\n";
    LogFile.Flush();
}
