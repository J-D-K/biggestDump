#include "Directory.hpp"
#include "ErrorCommon.h"
#include "FsLib.hpp"
#include "String.hpp"
#include <algorithm>
#include <cstring>


// fslib global error string.
extern std::string g_FsLibErrorString;

// Sorts by entry type, then alphabetically.
static bool CompareEntries(const FsDirectoryEntry &EntryA, const FsDirectoryEntry &EntryB)
{
    if (EntryA.type != EntryB.type)
    {
        return EntryA.type == FsDirEntryType_Dir;
    }

    size_t EntryALength = std::strlen(EntryA.name);
    size_t EntryBLength = std::strlen(EntryB.name);
    size_t ShortestString = EntryALength < EntryBLength ? EntryALength : EntryBLength;
    for (size_t i = 0; i < ShortestString; i++)
    {
        int CharA = std::tolower(EntryA.name[i]);
        int CharB = std::tolower(EntryB.name[i]);
        if (CharA != CharB)
        {
            return CharA < CharB;
        }
    }
    return false;
}

FsLib::Directory::Directory(const FsLib::Path &DirectoryPath)
{
    Directory::Open(DirectoryPath);
}

void FsLib::Directory::Open(const FsLib::Path &DirectoryPath)
{
    // Make sure this is false just in case directory is reused.
    m_WasRead = false;

    if (!DirectoryPath.IsValid())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return;
    }

    FsFileSystem *FileSystem;
    if (!FsLib::GetFileSystemByDeviceName(DirectoryPath.GetDeviceName(), &FileSystem))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return;
    }

    Result FsError =
        fsFsOpenDirectory(FileSystem, DirectoryPath.GetPath(), FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, &m_DirectoryHandle);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening directory: 0x%X.", FsError);
        return;
    }

    FsError = fsDirGetEntryCount(&m_DirectoryHandle, &m_EntryCount);
    if (R_FAILED(FsError))
    {
        Directory::Close();
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error getting directory entry count: 0x%X.", FsError);
        return;
    }

    // Read entries.
    int64_t TotalEntriesRead = 0;
    m_DirectoryList = std::make_unique<FsDirectoryEntry[]>(m_EntryCount); // This should free the previous array on allocation.
    FsError = fsDirRead(&m_DirectoryHandle, &TotalEntriesRead, m_EntryCount, m_DirectoryList.get());
    if (R_FAILED(FsError) || TotalEntriesRead != m_EntryCount)
    {
        Directory::Close();
        g_FsLibErrorString =
            FsLib::String::GetFormattedString("Error reading entries: 0x%X. %02d/%02d entries read.", FsError, TotalEntriesRead, m_EntryCount);
        return;
    }
    // Sort the array.
    std::sort(m_DirectoryList.get(), m_DirectoryList.get() + m_EntryCount, CompareEntries);
    // Close handle for sure
    Directory::Close();
    // We're good
    m_WasRead = true;
}

bool FsLib::Directory::IsOpen(void) const
{
    return m_WasRead;
}

int64_t FsLib::Directory::GetEntryCount(void) const
{
    return m_EntryCount;
}

int64_t FsLib::Directory::GetEntrySizeAt(int Index) const
{
    if (Index >= m_EntryCount)
    {
        return 0;
    }
    return m_DirectoryList[Index].file_size;
}

const char *FsLib::Directory::GetEntryAt(int Index) const
{
    if (Index >= m_EntryCount)
    {
        return nullptr;
    }
    return m_DirectoryList[Index].name;
}

bool FsLib::Directory::EntryAtIsDirectory(int Index) const
{
    if (Index >= m_EntryCount)
    {
        return false;
    }
    return m_DirectoryList[Index].type == FsDirEntryType_Dir;
}

const char *FsLib::Directory::operator[](int Index) const
{
    if (Index < 0 || Index > m_EntryCount)
    {
        return nullptr;
    }
    return m_DirectoryList[Index].name;
}

void FsLib::Directory::Close(void)
{
    fsDirClose(&m_DirectoryHandle);
}
