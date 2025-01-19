#include "FsLib.hpp"
#include "String.hpp"
#include <3ds.h>
#include <unordered_map>

namespace
{
    // 3DS can use UTF-16 paths so that's what we're using.
    std::unordered_map<std::u16string_view, FS_Archive> s_DeviceMap;
    // This only works because the string is so short.
    constexpr std::u16string_view SDMC_DEVICE_NAME = u"sdmc";
} // namespace

// Globally shared error string.
std::string g_FsLibErrorString = "No errors encountered.";

// Checks if device is already in map.
static bool DeviceNameIsInUse(std::u16string_view DeviceName)
{
    return s_DeviceMap.find(DeviceName) != s_DeviceMap.end();
}

bool FsLib::Initialize(void)
{
    Result FsError = fsInit();
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error initializing FS: 0x%08X.", FsError);
        return false;
    }

    FsError = FSUSER_OpenArchive(&s_DeviceMap[SDMC_DEVICE_NAME], ARCHIVE_SDMC, {PATH_EMPTY, 0x00, NULL});
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening SDMC Archive: 0x%08X.", FsError);
        return false;
    }

    return true;
}

void FsLib::Exit(void)
{
    for (auto &[DeviceName, Archive] : s_DeviceMap)
    {
        FSUSER_CloseArchive(Archive);
    }
    fsExit();
}

const char *FsLib::GetErrorString(void)
{
    return g_FsLibErrorString.c_str();
}

bool FsLib::MapArchiveToDevice(std::u16string_view DeviceName, FS_Archive Archive)
{
    if (DeviceName == u"sdmc")
    {
        return false;
    }

    if (DeviceNameIsInUse(DeviceName))
    {
        FsLib::CloseDevice(DeviceName);
    }

    // FS_Archive is just a uint64 handle so not gonna bother to memcpy like Switch to make sure I have everything.
    s_DeviceMap[DeviceName] = Archive;

    return true;
}

bool FsLib::GetArchiveByDeviceName(std::u16string_view DeviceName, FS_Archive *ArchiveOut)
{
    if (!DeviceNameIsInUse(DeviceName))
    {
        return false;
    }

    *ArchiveOut = s_DeviceMap[DeviceName];

    return true;
}

bool FsLib::ControlDevice(std::u16string_view DeviceName)
{
    if (!DeviceNameIsInUse(DeviceName))
    {
        return false;
    }

    Result FsError = FSUSER_ControlArchive(s_DeviceMap[DeviceName], ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error committing save to device: 0x%08X.", FsError);
        return false;
    }
    return true;
}

bool FsLib::CloseDevice(std::u16string_view DeviceName)
{
    if (!DeviceNameIsInUse(DeviceName))
    {
        return false;
    }

    Result FsError = FSUSER_CloseArchive(s_DeviceMap[DeviceName]);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error closing archive: 0x%08X.", FsError);
        return false;
    }
    // Erase the device from map so DeviceNameIsInUse works correctly.
    s_DeviceMap.erase(DeviceName);
    return true;
}
