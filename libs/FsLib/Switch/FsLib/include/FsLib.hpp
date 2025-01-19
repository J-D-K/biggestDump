#pragma once
#include "BisFileSystem.hpp"
#include "Dev.hpp"
#include "Directory.hpp"
#include "DirectoryFunctions.hpp"
#include "File.hpp"
#include "FileFunctions.hpp"
#include "Path.hpp"
#include "SaveFileSystem.hpp"
#include "Storage.hpp"
#include <string>
#include <switch.h>

namespace FsLib
{
    /// @brief Initializes FsLib. Steals LibNX's sdmc handle.
    /// @note Once FsLib::Dev is implemented for Switch this will get more interesting.
    /// @return True on success. False on failure.
    bool Initialize(void);

    /// @brief Exits FsLib closing any remaining open devices.
    void Exit(void);

    /// @brief Returns the internal error string for slightly more descriptive errors than a bool.
    /// @return Internal error string.
    const char *GetErrorString(void);

    /**
     * @brief Maps FileSystem to DeviceName internally.
     *
     * @param DeviceName Name to use for Device.
     * @param FileSystem FileSystem to map to DeviceName.
     * @return True on success. False on failure.
     * @note If a FileSystem is already mapped to DeviceName, it <b>will</b> be unmounted and replaced with FileSystem instead of just
     * returning NULL like fs_dev. There is also <b>no</b> real limit to how many devices you can have open besides the Switch handle limit.
     * fs_dev only allows 32 at a time.
     */
    bool MapFileSystem(std::string_view DeviceName, FsFileSystem *FileSystem);

    /// @brief Attempts to find Device in map.
    /// @param DeviceName Name of the Device to locate.
    /// @param FileSystemOut Set to pointer to FileSystem handle mapped to DeviceName.
    /// @return True if DeviceName is found, false if it isn't.
    /// @note This isn't really useful outside of internal FsLib functions, but I don't want to hide it like archive_dev does in ctrulib.
    bool GetFileSystemByDeviceName(std::string_view DeviceName, FsFileSystem **FileSystemOut);

    /// @brief Attempts to commit data to DeviceName.
    /// @param DeviceName Name of device to commit data to.
    /// @return True on success. False on failure.
    bool CommitDataToFileSystem(std::string_view DeviceName);

    /**
     * @brief Attempts to get the free space available on Device passed.
     *
     * @param DeviceRoot Root of device.
     * @param SizeOut The size retrieved if successful.
     * @return True on success. False on failure.
     * @note This function requires a path to work. DeviceRoot should be `sdmc:/` instead of `sdmc`, for example.
     */
    bool GetDeviceFreeSpace(const FsLib::Path &DeviceRoot, int64_t &SizeOut);

    /**
     * @brief Attempts to get the total space of Device passed.
     *
     * @param DeviceRoot Root of device.
     * @param SizeOut The size retrieved if successful.
     * @return True on success. False on failure.
     * @note This function requires a path to work. DeviceRoot should be `sdmc:/` instead of `sdmc`, for example.
     */
    bool GetDeviceTotalSpace(const FsLib::Path &DeviceRoot, int64_t &SizeOut);

    /// @brief Closes filesystem mapped to DeviceName
    /// @param DeviceName Name of device to close.
    /// @return True on success. False on Failure or device not found.
    bool CloseFileSystem(std::string_view DeviceName);
} // namespace FsLib
