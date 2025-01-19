#pragma once
#include "Path.hpp"

namespace FsLib
{
    /// @brief Attempts to create directory with DirectoryPath.
    /// @param DirectoryPath Path to new directory.
    /// @return True on success. False on failure.
    bool CreateDirectory(const FsLib::Path &DirectoryPath);

    /**
     * @brief Attempts to create all directories in path if possible. Path does not need a trailing slash.
     *
     * @param DirectoryPath Path of directories.
     * @return True on success. False on failure.
     * @note This can be kind of slow. At the moment, I'm not sure if there's a bug or Switch has some kind of limit on folder depth, but
     * after a certain point this will fail. Use only where <b>absolutely needed</b>.
     */
    bool CreateDirectoriesRecursively(const FsLib::Path &DirectoryPath);

    /// @brief Attempts to delete the directory passed.
    /// @param DirectoryPath Path to the target directory.
    /// @return True on success. False on failure.
    bool DeleteDirectory(const FsLib::Path &DirectoryPath);

    /// @brief Attempts to delete directory path recursively.
    /// @param DirectoryPath Path to target directory.
    /// @return True on success. False on failure.
    bool DeleteDirectoryRecursively(const FsLib::Path &DirectoryPath);

    /// @brief Attempts to open directory for reading to see if it exists.
    /// @param DirectoryPath Path to the target directory.
    /// @return True on success. False on failure.
    bool DirectoryExists(const FsLib::Path &DirectoryPath);

    /// @brief Attempts to rename OldPath to NewPath.
    /// @param OldPath Original path to target directory.
    /// @param NewPath New path to target directory.
    /// @return True on success. False on failure.
    bool RenameDirectory(const FsLib::Path &OldPath, const FsLib::Path &NewPath);
} // namespace FsLib
