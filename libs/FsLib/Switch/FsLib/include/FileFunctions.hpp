#pragma once
#include "Path.hpp"

namespace FsLib
{
    /// @brief Attempts to create file with FileSize.
    /// @param FilePath Path to file to create.
    /// @param FileSize Optional. The size to use when creating the file.
    /// @return True on success. False on failure.
    bool CreateFile(const FsLib::Path &FilePath, int64_t FileSize = 0);

    /// @brief Checks to see if the file exists.
    /// @param FilePath Path of target file.
    /// @return True if it exists. False if it doesn't.
    bool FileExists(const FsLib::Path &FilePath);

    /// @brief Attempts to delete file.
    /// @param FilePath Path of target file.
    /// @return True on success. False on failure.
    bool DeleteFile(const FsLib::Path &FilePath);

    /// @brief Attempts to get file's size.
    /// @param FilePath Path of target file.
    /// @return File's size on success. -1 on error.
    int64_t GetFileSize(const FsLib::Path &FilePath);

    /// @brief Attempts to rename OldPath to NewPath.
    /// @param OldPath Original path of target file.
    /// @param NewPath New path of target file.
    /// @return True on success. False on failure.
    bool RenameFile(const FsLib::Path &OldPath, const FsLib::Path &NewPath);
} // namespace FsLib
