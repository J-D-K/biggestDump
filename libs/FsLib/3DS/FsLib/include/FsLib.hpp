#pragma once
#include "Dev.hpp"
#include "Directory.hpp"
#include "DirectoryFunctions.hpp"
#include "File.hpp"
#include "FileFunctions.hpp"
#include "Path.hpp"
#include "SaveDataArchive.hpp"
#include <3ds.h>
#include <string>

namespace FsLib
{
    /// @brief Opens and mounts SD card to u"sdmc:/"
    /// @return True on success. False on failure.
    bool Initialize(void);

    /// @brief Exits and closes all open handles.
    void Exit(void);

    /// @brief Returns internal error string with slightly more information than just a bool can provide.
    /// @return Error string.
    const char *GetErrorString(void);

    /// @brief Adds Archive to devices.
    /// @param DeviceName Name of the device. Ex: u"sdmc".
    /// @param Archive Archive to map.
    /// @return True on success. False on failure.
    bool MapArchiveToDevice(std::u16string_view DeviceName, FS_Archive Archive);

    /// @brief Attempts to retrieve the archive mapped to DeviceName.
    /// @param DeviceName Name of the archive to retrieve. Ex: u"sdmc"
    /// @param ArchiveOut Pointer to Archive to write to.
    /// @return True if the archive is found. False if it is not.
    bool GetArchiveByDeviceName(std::u16string_view DeviceName, FS_Archive *ArchiveOut);

    /// @brief Performs control on DeviceName AKA commits data to it. This is not required for Extra Data types or SDMC.
    /// @param DeviceName Name of the device to control.
    /// @return True on success. False on failure.
    bool ControlDevice(std::u16string_view DeviceName);

    /// @brief Closes the archive mapped to DeviceName.
    /// @param DeviceName Name of the device to close.
    /// @return True on success. False on failure.
    bool CloseDevice(std::u16string_view DeviceName);
} // namespace FsLib
