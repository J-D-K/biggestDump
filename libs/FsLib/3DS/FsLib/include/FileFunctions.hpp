#pragma once
#include "Path.hpp"

namespace FsLib
{
    /// @brief This is a shortcut function to create an empty file.
    /// @param FilePath Path to file to create.
    /// @param FileSize Size of file.
    /// @return True on success. False on failure.
    bool CreateFile(const FsLib::Path &FilePath, uint64_t FileSize);

    /// @brief Attempts to open FilePath for reading to check if it exists.
    /// @param FilePath Path to file to check.
    /// @return True if file exists. False if it doesn't.
    bool FileExists(const FsLib::Path &FilePath);

    /// @brief Attempts to rename file from OldPath to NewPath. Both must exist on the same device.
    /// @param OldPath Original path to target file.
    /// @param NewPath New path of target file.
    /// @return True on success. False on failure.
    bool RenameFile(const FsLib::Path &OldPath, const FsLib::Path &NewPath);

    /// @brief Attempts to delete file.
    /// @param FilePath Path of target file.
    /// @return True on success. False on failure.
    bool DeleteFile(const FsLib::Path &FilePath);
} // namespace FsLib
