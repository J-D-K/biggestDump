#pragma once
#include <cstdint>
#include <string>
#include <switch.h>

namespace FsLib
{
    /// @brief Attempts to open system save data with SystemSaveID and map it to DeviceName.
    /// @param DeviceName Name of device to map to.
    /// @param SystemSaveID ID of system save.
    /// @return True on success. False on failure.
    bool OpenSystemSaveFileSystem(std::string_view DeviceName, uint64_t SystemSaveID);

    /// @brief Attempts to open account save data with ApplicationID and UserID and map it to DeviceName.
    /// @param DeviceName Name of device to map to.
    /// @param ApplicationID Title ID of game or title.
    /// @param UserID Account ID of user that "owns" the filesystem.
    /// @return True on success. False on failure.
    bool OpenAccountSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID, AccountUid UserID);

    /// @brief Attempts to open BCAT save data with ApplicationID and map it to DeviceName.
    /// @param DeviceName Name of device to map to.
    /// @param ApplicationID Title ID of game or title.
    /// @return True on success. False on failure.
    bool OpenBCATSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID);

    /// @brief Attempts to open device save data with ApplicationID and map it to DeviceName.
    /// @param DeviceName Name of device to map to.
    /// @param ApplicationID Title ID of game or title.
    /// @return True on success. False on failure.
    bool OpenDeviceSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID);

    /// @brief Attempts to open temporary save data and map it to DeviceName.
    /// @param DeviceName Name of device to map to.
    /// @return True on success. False on failure.
    /// @note I've only ever seen this used once before...
    bool OpenTemporarySaveFileSystem(std::string_view DeviceName);

    /// @brief Attempts to open cache save data and map it to DeviceName.
    /// @param DeviceName Name of device to map to.
    /// @param ApplicationID Title ID of game or title.
    /// @param SaveIndex Index of the cache save.
    /// @return True on success. False on failure.
    bool OpenCacheSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID, uint16_t SaveIndex);

    /// @brief Attempts to open system BCAT save data and map it to DeviceName.
    /// @param DeviceName Name of device to map to.
    /// @param SystemSaveID ID of system save.
    /// @return True on success. False on failure.
    /// @note I've never seen this used before. Literally <b>ever</b>.
    bool OpenSystemBCATSaveFileSystem(std::string_view DeviceName, uint64_t SystemSaveID);
} // namespace FsLib
