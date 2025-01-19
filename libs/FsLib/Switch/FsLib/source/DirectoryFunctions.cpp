#include "DirectoryFunctions.hpp"
#include "ErrorCommon.h"
#include "FsLib.hpp"
#include "String.hpp"
#include <switch.h>

extern std::string g_FsLibErrorString;

bool FsLib::CreateDirectory(const FsLib::Path &DirectoryPath)
{
    if (!DirectoryPath.IsValid())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *FileSystem;
    if (!FsLib::GetFileSystemByDeviceName(DirectoryPath.GetDeviceName(), &FileSystem))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result FsError = fsFsCreateDirectory(FileSystem, DirectoryPath.GetPath());
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error creating directory %s: 0x%X.", DirectoryPath.CString(), FsError);
        return false;
    }

    return true;
}

bool FsLib::CreateDirectoriesRecursively(const FsLib::Path &DirectoryPath)
{
    size_t SlashPosition = DirectoryPath.FindFirstOf('/') + 1;
    do
    {
        SlashPosition = DirectoryPath.FindFirstOf('/', SlashPosition);
        FsLib::Path CurrentDirectory = DirectoryPath.SubPath(SlashPosition);
        if (!FsLib::DirectoryExists(CurrentDirectory) && !FsLib::CreateDirectory(CurrentDirectory))
        {
            // Create directory will set the error string.
            return false;
        }
        ++SlashPosition;
    } while (SlashPosition < DirectoryPath.GetLength()); // There might be another way to do this, but this works for now.
    return true;
}

bool FsLib::DeleteDirectory(const FsLib::Path &DirectoryPath)
{
    if (!DirectoryPath.IsValid())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *FileSystem;
    if (!FsLib::GetFileSystemByDeviceName(DirectoryPath.GetDeviceName(), &FileSystem))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result FsError = fsFsDeleteDirectory(FileSystem, DirectoryPath.GetPath());
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error deleting directory: 0x%X.", FsError);
        return false;
    }
    return true;
}

bool FsLib::DeleteDirectoryRecursively(const FsLib::Path &DirectoryPath)
{
    FsLib::Directory TargetDirectory(DirectoryPath);
    if (!TargetDirectory.IsOpen())
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error deleting directory recursively: %s", g_FsLibErrorString.c_str());
        return false;
    }

    for (int64_t i = 0; i < TargetDirectory.GetEntryCount(); i++)
    {
        FsLib::Path TargetPath = DirectoryPath / TargetDirectory[i];
        if (TargetDirectory.EntryAtIsDirectory(i) && !FsLib::DeleteDirectoryRecursively(TargetPath))
        {
            return false;
        }
        else if (!TargetDirectory.EntryAtIsDirectory(i) && !FsLib::DeleteFile(TargetPath))
        {
            return false;
        }
    }
    // Nintendo's implementation of this function will try to delete the root and report failure. Mine doesn't.
    auto PathBegin = std::char_traits<char>::find(DirectoryPath.CString(), DirectoryPath.GetLength(), '/');
    if (std::char_traits<char>::length(PathBegin) > 1 && !FsLib::DeleteDirectory(DirectoryPath))
    {
        return false;
    }
    return true;
}

bool FsLib::DirectoryExists(const FsLib::Path &DirectoryPath)
{
    if (!DirectoryPath.IsValid())
    {
        // Not going to bother setting error string for this.
        return false;
    }

    FsFileSystem *FileSystem;
    if (!FsLib::GetFileSystemByDeviceName(DirectoryPath.GetDeviceName(), &FileSystem))
    {
        return false;
    }

    FsDir DirectoryHandle;
    Result FsError = fsFsOpenDirectory(FileSystem, DirectoryPath.GetPath(), FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, &DirectoryHandle);
    if (R_FAILED(FsError))
    {
        return false;
    }
    fsDirClose(&DirectoryHandle);
    return true;
}

bool FsLib::RenameDirectory(const FsLib::Path &OldPath, const FsLib::Path &NewPath)
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

    Result FsError = fsFsRenameDirectory(FileSystem, OldPath.GetPath(), NewPath.GetPath());
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error renaming directory: 0x%X.", FsError);
        return false;
    }

    return true;
}
