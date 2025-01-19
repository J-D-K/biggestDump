#include "SaveFileSystem.hpp"
#include "FsLib.hpp"
#include "String.hpp"
#include <switch.h>

extern std::string g_FsLibErrorString;

bool FsLib::OpenSystemSaveFileSystem(std::string_view DeviceName, uint64_t SystemSaveID)
{
    FsSaveDataAttribute SaveDataAttributes = {.application_id = 0,
                                              .uid = {0},
                                              .system_save_data_id = SystemSaveID,
                                              .save_data_type = FsSaveDataType_System,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    FsFileSystem FileSystem;
    Result FsError = fsOpenSaveDataFileSystem(&FileSystem, FsSaveDataSpaceId_User, &SaveDataAttributes);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening system save data: 0x%X.", FsError);
        return false;
    }

    if (!FsLib::MapFileSystem(DeviceName, &FileSystem))
    {
        fsFsClose(&FileSystem);
        return false;
    }

    return true;
}

bool FsLib::OpenAccountSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID, AccountUid UserID)
{
    FsSaveDataAttribute SaveDataAttributes = {.application_id = ApplicationID,
                                              .uid = UserID,
                                              .system_save_data_id = 0,
                                              .save_data_type = FsSaveDataType_Account,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    FsFileSystem FileSystem;
    Result FsError = fsOpenSaveDataFileSystem(&FileSystem, FsSaveDataSpaceId_User, &SaveDataAttributes);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening account save data: 0x%X.", FsError);
        return false;
    }

    if (!FsLib::MapFileSystem(DeviceName, &FileSystem))
    {
        fsFsClose(&FileSystem);
        return false;
    }

    return true;
}

bool FsLib::OpenBCATSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID)
{
    FsSaveDataAttribute SaveDataAttributes = {.application_id = ApplicationID,
                                              .uid = {0},
                                              .system_save_data_id = 0,
                                              .save_data_type = FsSaveDataType_Bcat,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    FsFileSystem FileSystem;
    Result FsError = fsOpenSaveDataFileSystem(&FileSystem, FsSaveDataSpaceId_User, &SaveDataAttributes);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening BCAT save data: 0x%X.", FsError);
        return false;
    }

    if (!FsLib::MapFileSystem(DeviceName, &FileSystem))
    {
        fsFsClose(&FileSystem);
        return false;
    }

    return true;
}

bool FsLib::OpenDeviceSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID)
{
    FsSaveDataAttribute SaveDataAttributes = {.application_id = ApplicationID,
                                              .uid = {0},
                                              .system_save_data_id = 0,
                                              .save_data_type = FsSaveDataType_Device,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    FsFileSystem FileSystem;
    Result FsError = fsOpenSaveDataFileSystem(&FileSystem, FsSaveDataSpaceId_User, &SaveDataAttributes);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening device save data: 0x%X.", FsError);
        return false;
    }

    if (!FsLib::MapFileSystem(DeviceName, &FileSystem))
    {
        fsFsClose(&FileSystem);
        return false;
    }

    return true;
}

bool FsLib::OpenTemporarySaveFileSystem(std::string_view DeviceName)
{
    FsSaveDataAttribute SaveDataAttributes = {.application_id = 0,
                                              .uid = {0},
                                              .system_save_data_id = 0,
                                              .save_data_type = FsSaveDataType_Temporary,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    FsFileSystem FileSystem;
    Result FsError = fsOpenSaveDataFileSystem(&FileSystem, FsSaveDataSpaceId_User, &SaveDataAttributes);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening temporary save data: 0x%X.", FsError);
        return false;
    }

    if (!FsLib::MapFileSystem(DeviceName, &FileSystem))
    {
        fsFsClose(&FileSystem);
        return false;
    }

    return true;
}

bool FsLib::OpenCacheSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID, uint16_t SaveIndex)
{
    FsSaveDataAttribute SaveDataAttributes = {.application_id = ApplicationID,
                                              .uid = {0},
                                              .system_save_data_id = 0,
                                              .save_data_type = FsSaveDataType_Cache,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = SaveIndex};

    FsFileSystem FileSystem;
    Result FsError = fsOpenSaveDataFileSystem(&FileSystem, FsSaveDataSpaceId_User, &SaveDataAttributes);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening cache save data: 0x%X.", FsError);
        return false;
    }

    if (!FsLib::MapFileSystem(DeviceName, &FileSystem))
    {
        fsFsClose(&FileSystem);
        return false;
    }

    return true;
}

bool FsLib::OpenSystemBCATSaveFileSystem(std::string_view DeviceName, uint64_t SystemSaveID)
{
    FsSaveDataAttribute SaveDataAttributes = {.application_id = 0,
                                              .uid = {0},
                                              .system_save_data_id = SystemSaveID,
                                              .save_data_type = FsSaveDataType_SystemBcat,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    FsFileSystem FileSystem;
    Result FsError = fsOpenSaveDataFileSystem(&FileSystem, FsSaveDataSpaceId_User, &SaveDataAttributes);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening system bcat save data: 0x%X.", FsError);
        return false;
    }

    if (!FsLib::MapFileSystem(DeviceName, &FileSystem))
    {
        fsFsClose(&FileSystem);
        return false;
    }

    return true;
}
