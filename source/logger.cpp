#include "logger.hpp"
#include "fslib.hpp"
#include <cstdarg>

namespace
{
    // Size for formatted strings.
    constexpr size_t VA_BUFFER_SIZE = 0x1000;
    // Path for the log file.
    const char *LOG_FILE_PATH = "sdmc:/switch/biggestDump.log";
} // namespace

void logger::initialize(void)
{
    fslib::File logFile(LOG_FILE_PATH, FsOpenMode_Create);
}

void logger::log(const char *format, ...)
{
    char vaBuffer[VA_BUFFER_SIZE] = {0};

    std::va_list vaList;
    va_start(vaList, format);
    vsnprintf(vaBuffer, VA_BUFFER_SIZE, format, vaList);
    va_end(vaList);

    fslib::File logFile(LOG_FILE_PATH, FsOpenMode_Append);
    logFile << vaBuffer << "\n";
    logFile.flush();
}
