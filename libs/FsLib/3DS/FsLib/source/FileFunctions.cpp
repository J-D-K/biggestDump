#include "FileFunctions.hpp"
#include "ErrorCommon.h"
#include "FsLib.hpp"
#include "String.hpp"

extern std::string g_FsLibErrorString;

bool FsLib::CreateFile(const FsLib::Path &FilePath, uint64_t FileSize)
{
    if (!FilePath.IsValid())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FS_Archive Archive;
    if (!FsLib::GetArchiveByDeviceName(FilePath.GetDevice(), &Archive))
    {
        return false;
    }

    Result FsError = FSUSER_CreateFile(Archive, FilePath.GetPath(), 0, FileSize);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error creating file: 0x%08X.", FsError);
        return false;
    }

    return true;
}

bool FsLib::FileExists(const FsLib::Path &FilePath)
{
    if (!FilePath.IsValid())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FS_Archive Archive;
    if (!FsLib::GetArchiveByDeviceName(FilePath.GetDevice(), &Archive))
    {
        return false;
    }

    Handle FileHandle;
    Result FsError = FSUSER_OpenFile(&FileHandle, Archive, FilePath.GetPath(), FS_OPEN_READ, 0);
    if (R_FAILED(FsError))
    {
        return false;
    }

    FSFILE_Close(FileHandle);
    return true;
}

bool FsLib::RenameFile(const FsLib::Path &OldPath, const FsLib::Path &NewPath)
{
    if ((!OldPath.IsValid() || !NewPath.IsValid()) || (OldPath.GetDevice() != NewPath.GetDevice()))
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FS_Archive Archive;
    if (!FsLib::GetArchiveByDeviceName(OldPath.GetDevice(), &Archive))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result FsError = FSUSER_RenameFile(Archive, OldPath.GetPath(), Archive, NewPath.GetPath());
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error renaming file: 0x%08X.", FsError);
        return false;
    }
    return true;
}

bool FsLib::DeleteFile(const FsLib::Path &FilePath)
{
    if (!FilePath.IsValid())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FS_Archive Archive;
    if (!FsLib::GetArchiveByDeviceName(FilePath.GetDevice(), &Archive))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result FsError = FSUSER_DeleteFile(Archive, FilePath.GetPath());
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error deleting file: 0x%08X.", FsError);
        return false;
    }
    return true;
}
