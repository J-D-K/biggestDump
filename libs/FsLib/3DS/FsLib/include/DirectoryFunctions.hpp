#pragma once
#include "Path.hpp"

namespace FsLib
{
    /// @brief Attempts to open directory at Directory path to see if it exists and is a directory.
    /// @param DirectoryPath Path to test.
    /// @return True if the directory exists and can be opened. False if it does not.
    bool DirectoryExists(const FsLib::Path &DirectoryPath);

    /// @brief Attempts to create directory with DirectoryPath.
    /// @param DirectoryPath Path to create.
    /// @return True on success. False on failure.
    bool CreateDirectory(const FsLib::Path &DirectoryPath);

    /// @brief Attempts to create every directory in Directory path. Does not need a trailing slash. Warning: This is a bit slow.
    /// @return True on success. False on failure.
    bool CreateDirectoriesRecursively(const FsLib::Path &DirectoryPath);

    /// @brief Attempts to rename directory from OldPath to NewPath. Both must be on the same device.
    /// @param OldPath Original name of target directory.
    /// @param NewPath New name of target directory.
    /// @return True on success. False on failure.
    bool RenameDirectory(const FsLib::Path &OldPath, const FsLib::Path &NewPath);

    /// @brief Attempts to delete the directory passed in DirectoryPath.
    /// @param DirectoryPath Target directory path.
    /// @return True on success. False on failure.
    bool DeleteDirectory(const FsLib::Path &DirectoryPath);

    /// @brief Attempts to recursively delete DirectoryPath
    /// @param DirectoryPath Path to the directory to delete.
    /// @return True on success. False on failure.
    bool DeleteDirectoryRecursively(const FsLib::Path &DirectoryPath);
} // namespace FsLib
