#include "BisFileSystem.hpp"
#include "FsLib.hpp"
#include "String.hpp"

extern std::string g_FsLibErrorString;

bool FsLib::OpenBisFileSystem(std::string_view DeviceName, FsBisPartitionId PartitionID)
{
    FsFileSystem FileSystem;
    Result FsError = fsOpenBisFileSystem(&FileSystem, PartitionID, "");
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening BisFileSystem: 0x%X.", FsError);
        return false;
    }

    if (!FsLib::MapFileSystem(DeviceName, &FileSystem))
    {
        fsFsClose(&FileSystem);
        return false;
    }
    return true;
}
