#include "SaveDataArchive.hpp"
#include "FsLib.hpp"
#include "String.hpp"

extern std::string g_FsLibErrorString;

bool FsLib::OpenSaveData(std::u16string_view DeviceName)
{
    FS_Archive Archive;
    Result FsError = FSUSER_OpenArchive(&Archive, ARCHIVE_SAVEDATA, {PATH_EMPTY, 0x00, NULL});
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening save data archive: 0x%08X.", FsError);
        return false;
    }

    if (!FsLib::MapArchiveToDevice(DeviceName, Archive))
    {
        return false;
    }
    return true;
}

bool FsLib::OpenExtData(std::u16string_view DeviceName, uint32_t ExtDataID)
{
    FS_Archive Archive;
    uint32_t BinaryData[] = {MEDIATYPE_SD, ExtDataID, 0x00000000};
    FS_Path PathData = {.type = PATH_BINARY, .size = 0x0C, .data = BinaryData};

    Result FsError = FSUSER_OpenArchive(&Archive, ARCHIVE_EXTDATA, PathData);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening ExtData archive %08X: 0x%08X.", ExtDataID, FsError);
        return false;
    }

    if (!FsLib::MapArchiveToDevice(DeviceName, Archive))
    {
        FSUSER_CloseArchive(Archive);
        return false;
    }

    return true;
}

bool FsLib::OpenSharedExtData(std::u16string_view DeviceName, uint32_t SharedExtDataID)
{
    FS_Archive Archive;
    uint32_t BinaryData[] = {MEDIATYPE_NAND, SharedExtDataID, 0x00048000};
    FS_Path PathData = {.type = PATH_BINARY, .size = 0x0C, .data = BinaryData};
    Result FsError = FSUSER_OpenArchive(&Archive, ARCHIVE_SHARED_EXTDATA, PathData);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening shared extdata %08X: 0x%08X.", SharedExtDataID, FsError);
        return false;
    }

    if (!FsLib::MapArchiveToDevice(DeviceName, Archive))
    {
        FSUSER_CloseArchive(Archive);
        return false;
    }

    return true;
}

// This is basically identical to OpenExtData, but the archive ID is different.
bool FsLib::OpenBossExtData(std::u16string_view DeviceName, uint32_t ExtDataID)
{
    FS_Archive Archive;
    uint32_t BinaryData[] = {MEDIATYPE_SD, ExtDataID, 0x00000000};
    FS_Path PathData = {.type = PATH_BINARY, .size = 0x0C, .data = BinaryData};

    Result FsError = FSUSER_OpenArchive(&Archive, ARCHIVE_BOSS_EXTDATA, PathData);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening BOSS Extdata archive %08X: 0x%08X", ExtDataID, FsError);
        return false;
    }

    if (!FsLib::MapArchiveToDevice(DeviceName, Archive))
    {
        FSUSER_CloseArchive(Archive);
        return false;
    }
    return true;
}

bool FsLib::OpenSystemSaveData(std::u16string_view DeviceName, uint32_t UniqueID)
{
    FS_Archive Archive;
    uint32_t BinaryData[] = {MEDIATYPE_NAND, 0x00020000 | UniqueID};
    FS_Path PathData = {.type = PATH_BINARY, .size = 0x08, .data = BinaryData};
    Result FsError = FSUSER_OpenArchive(&Archive, ARCHIVE_SYSTEM_SAVEDATA, PathData);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening system save data %08X: 0x%08.", UniqueID, FsError);
        return false;
    }

    if (!FsLib::MapArchiveToDevice(DeviceName, Archive))
    {
        FSUSER_CloseArchive(Archive);
        return false;
    }

    return true;
}

bool FsLib::OpenGameCardSaveData(std::u16string_view DeviceName)
{
    FS_Archive Archive;
    Result FsError = FSUSER_OpenArchive(&Archive, ARCHIVE_GAMECARD_SAVEDATA, {PATH_EMPTY, 0x00, NULL});
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening game card save data: 0x%08X.", FsError);
        return false;
    }

    if (!FsLib::MapArchiveToDevice(DeviceName, Archive))
    {
        FSUSER_CloseArchive(Archive);
        return false;
    }
    return true;
}

bool FsLib::OpenUserSaveData(std::u16string_view DeviceName, FS_MediaType MediaType, uint32_t LowerID, uint32_t UpperID)
{
    FS_Archive Archive;
    uint32_t BinaryData[] = {MediaType, LowerID, UpperID};
    FS_Path PathData = {.type = PATH_BINARY, .size = 0x0C, .data = BinaryData};
    Result FsError = FSUSER_OpenArchive(&Archive, ARCHIVE_USER_SAVEDATA, PathData);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening user save data %08X%08X: 0x%08X.", UpperID, LowerID, FsError);
        return false;
    }

    if (!FsLib::MapArchiveToDevice(DeviceName, Archive))
    {
        FSUSER_CloseArchive(Archive);
        return false;
    }

    return true;
}
