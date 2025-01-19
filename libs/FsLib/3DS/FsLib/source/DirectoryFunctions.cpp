#include "DirectoryFunctions.hpp"
#include "Directory.hpp"
#include "ErrorCommon.h"
#include "FsLib.hpp"
#include "String.hpp"
#include <3ds.h>

extern std::string g_FsLibErrorString;

bool FsLib::DirectoryExists(const FsLib::Path &DirectoryPath)
{
    if (!DirectoryPath.IsValid())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FS_Archive Archive;
    if (!FsLib::GetArchiveByDeviceName(DirectoryPath.GetDevice(), &Archive))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Handle DirectoryHandle;
    Result FsError = FSUSER_OpenDirectory(&DirectoryHandle, Archive, DirectoryPath.GetPath());
    if (R_FAILED(FsError))
    {
        return false;
    }
    FSDIR_Close(DirectoryHandle);
    return true;
}

bool FsLib::CreateDirectory(const FsLib::Path &DirectoryPath)
{
    if (!DirectoryPath.IsValid())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FS_Archive Archive;
    if (!FsLib::GetArchiveByDeviceName(DirectoryPath.GetDevice(), &Archive))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result FsError = FSUSER_CreateDirectory(Archive, DirectoryPath.GetPath(), 0);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error creating directory: 0x%08X.", FsError);
        return false;
    }
    return true;
}

bool FsLib::CreateDirectoriesRecursively(const FsLib::Path &DirectoryPath)
{
    size_t SlashPosition = DirectoryPath.FindFirstOf(u'/') + 1;

    do
    {
        SlashPosition = DirectoryPath.FindFirstOf(u'/', SlashPosition);
        FsLib::Path CurrentDirectory = DirectoryPath.SubPath(SlashPosition);
        if (!FsLib::DirectoryExists(CurrentDirectory) && !FsLib::CreateDirectory(CurrentDirectory))
        {
            // CreateDirectory will set the error string.
            return false;
        }
        ++SlashPosition;
    } while (SlashPosition < DirectoryPath.GetLength());
    return true;
}

bool FsLib::RenameDirectory(const FsLib::Path &OldPath, const FsLib::Path &NewPath)
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

    Result FsError = FSUSER_RenameDirectory(Archive, OldPath.GetPath(), Archive, NewPath.GetPath());
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error renaming directory: 0x%08X.", FsError);
        return false;
    }
    return true;
}

bool FsLib::DeleteDirectory(const FsLib::Path &DirectoryPath)
{
    if (!DirectoryPath.IsValid())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FS_Archive Archive;
    if (!FsLib::GetArchiveByDeviceName(DirectoryPath.GetDevice(), &Archive))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result FsError = FSUSER_DeleteDirectory(Archive, DirectoryPath.GetPath());
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error deleting directory: 0x%08X.", FsError);
        return false;
    }

    return true;
}

bool FsLib::DeleteDirectoryRecursively(const FsLib::Path &DirectoryPath)
{
    FsLib::Directory TargetDirectory(DirectoryPath);
    if (!TargetDirectory.IsOpen())
    {
        // We're going to do this because
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error deleting directory recursively: %s", g_FsLibErrorString.c_str());
        return false;
    }

    for (uint32_t i = 0; i < TargetDirectory.GetEntryCount(); i++)
    {
        FsLib::Path NewTarget = DirectoryPath / TargetDirectory[i];
        if (TargetDirectory.EntryAtIsDirectory(i) && !FsLib::DeleteDirectoryRecursively(NewTarget))
        {
            return false;
        }
        else if (!TargetDirectory.EntryAtIsDirectory(i) && !FsLib::DeleteFile(NewTarget))
        {
            return false;
        }
    }
    /*
        Make sure we're not trying to delete a device root before returning failure. I think this is what's wrong with Nintendo's implementation
        and why it fails when called on the root.
    */
    auto PathBegin = std::char_traits<char16_t>::find(DirectoryPath.CString(), DirectoryPath.GetLength(), u'/');
    if (std::char_traits<char16_t>::length(PathBegin) > 1 && !FsLib::DeleteDirectory(DirectoryPath))
    {
        return false;
    }
    return true;
}
