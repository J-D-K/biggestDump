#pragma once
#include <3ds.h>
#include <string>

namespace FsLib
{
    /// @brief Opens save archive of current title. This is only really useful for old hax entrypoints.
    /// @param DeviceName Name of device to map to use. Ex: u"SaveData"
    /// @return True on success. False on failure.
    bool OpenSaveData(std::u16string_view DeviceName);

    /// @brief Opens Extra Data archive associated with ExtDataID and maps it to DeviceName
    /// @param DeviceName Name of device to use. Ex: u"ExtData"
    /// @param ExtDataID ID of archive to open.
    /// @return True on success. False on failure.
    bool OpenExtData(std::u16string_view DeviceName, uint32_t ExtDataID);

    /// @brief Opens Shared Extra Data archive and maps it to DeviceName
    /// @param DeviceName Name of device to use. Ex: u"SharedExtData"
    /// @param SharedExtDataID ID of archive to open.
    /// @return True on success. False on failure.
    bool OpenSharedExtData(std::u16string_view DeviceName, uint32_t SharedExtDataID);

    /// @brief Opens BOSS Extra Data archive and maps it to DeviceName
    /// @param DeviceName Name of device to use. Ex: u"BossExtData"
    /// @param ExtDataID ID of archive to open.
    /// @return True on success. False on failure.
    bool OpenBossExtData(std::u16string_view DeviceName, uint32_t ExtDataID);

    /// @brief Opens system save data archive and maps it to DeviceName
    /// @param DeviceName Name of device to use. Ex: u"SystemSave"
    /// @param UniqueID ID of archive to open.
    /// @return True on success. False on failure.
    bool OpenSystemSaveData(std::u16string_view DeviceName, uint32_t UniqueID);

    /// @brief Opens the save data for the inserted game card and maps it to DeviceName.
    /// @param DeviceName Name of device to use. Ex: u"GameCard"
    /// @return True on success. False on failure.
    bool OpenGameCardSaveData(std::u16string_view DeviceName);

    /// @brief Opens user save data and maps it to DeviceName.
    /// @param DeviceName Name of device to use. Ex: u"UserSave"
    /// @param MediaType Media type of target title.
    /// @param LowerID Lower 32 bits of the title ID of the title. Ex: TitleID & 0xFFFFFFFF
    /// @param UpperID Upper 32 bites of the title ID of the title. Ex: TitleID >> 32 & 0xFFFFFFFF
    /// @return
    bool OpenUserSaveData(std::u16string_view DeviceName, FS_MediaType MediaType, uint32_t LowerID, uint32_t UpperID);
} // namespace FsLib
