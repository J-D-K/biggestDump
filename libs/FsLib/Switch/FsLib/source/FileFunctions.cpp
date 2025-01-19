#include "FileFunctions.hpp"
#include "ErrorCommon.h"
#include "FsLib.hpp"
#include "String.hpp"
#include <switch.h>

extern std::string g_FsLibErrorString;

bool FsLib::CreateFile(const FsLib::Path &FilePath, int64_t FileSize)
{
    if (!FilePath.IsValid())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *FileSystem;
    if (!FsLib::GetFileSystemByDeviceName(FilePath.GetDeviceName(), &FileSystem))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result FsError = fsFsCreateFile(FileSystem, FilePath.GetPath(), FileSize, 0);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error creating file: 0x%X.", FsError);
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

    FsFileSystem *FileSystem;
    if (!FsLib::GetFileSystemByDeviceName(FilePath.GetDeviceName(), &FileSystem))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    FsFile FileHandle;
    Result FsError = fsFsOpenFile(FileSystem, FilePath.GetPath(), FsOpenMode_Read, &FileHandle);
    if (R_FAILED(FsError))
    {
        return false;
    }
    fsFileClose(&FileHandle);
    return true;
}

bool FsLib::DeleteFile(const FsLib::Path &FilePath)
{
    if (!FilePath.IsValid())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *FileSystem;
    if (!FsLib::GetFileSystemByDeviceName(FilePath.GetDeviceName(), &FileSystem))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result FsError = fsFsDeleteFile(FileSystem, FilePath.GetPath());
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error deleting file: 0x%X.", FsError);
        return false;
    }
    return true;
}

int64_t FsLib::GetFileSize(const FsLib::Path &FilePath)
{
    if (!FilePath.IsValid())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return -1;
    }

    FsFileSystem *FileSystem;
    if (!FsLib::GetFileSystemByDeviceName(FilePath.GetDeviceName(), &FileSystem))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return -1;
    }

    FsFile FileHandle;
    Result FsError = fsFsOpenFile(FileSystem, FilePath.GetPath(), FsOpenMode_Read, &FileHandle);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening file to get size: 0x%X.", FsError);
        return -1;
    }

    int64_t FileSize = 0;
    FsError = fsFileGetSize(&FileHandle, &FileSize);
    if (R_FAILED(FsError))
    {
        fsFileClose(&FileHandle);
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error getting file size: 0x%X.", FsError);
        return -1;
    }

    fsFileClose(&FileHandle);
    return FileSize;
}

bool FsLib::RenameFile(const FsLib::Path &OldPath, const FsLib::Path &NewPath)
{
    if (!OldPath.IsValid() || !NewPath.IsValid() || OldPath.GetDeviceName() != NewPath.GetDeviceName())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *FileSystem;
    if (!FsLib::GetFileSystemByDeviceName(OldPath.GetDeviceName(), &FileSystem))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result FsError = fsFsRenameFile(FileSystem, OldPath.GetPath(), NewPath.GetPath());
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error renaming file: 0x%X.", FsError);
        return false;
    }
    return true;
}
