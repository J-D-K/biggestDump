#include "Directory.hpp"
#include "ErrorCommon.h"
#include "FsLib.hpp"
#include "String.hpp"
#include <algorithm>
#include <cstring>
#include <string>

bool CompareEntries(const FS_DirectoryEntry &EntryA, const FS_DirectoryEntry &EntryB)
{
    if (EntryA.attributes != EntryB.attributes)
    {
        return EntryA.attributes & FS_ATTRIBUTE_DIRECTORY;
    }

    size_t EntryALength = std::char_traits<uint16_t>::length(EntryA.name);
    size_t EntryBLength = std::char_traits<uint16_t>::length(EntryB.name);
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

extern std::string g_FsLibErrorString;

FsLib::Directory::Directory(const FsLib::Path &DirectoryPath)
{
    Directory::Open(DirectoryPath);
}

void FsLib::Directory::Open(const FsLib::Path &DirectoryPath)
{
    // Just in case directory is reused.
    m_WasOpened = false;
    m_DirectoryList.clear();

    if (!DirectoryPath.IsValid())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return;
    }

    FS_Archive Archive;
    if (!FsLib::GetArchiveByDeviceName(DirectoryPath.GetDevice(), &Archive))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return;
    }

    Result FsError = FSUSER_OpenDirectory(&m_DirectoryHandle, Archive, DirectoryPath.GetPath());
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening directory: 0x%08X.", FsError);
        return;
    }

    // Switch has a function to fetch entry count. 3DS doesn't, so we have to loop and load one at a time.
    uint32_t EntriesRead = 0;
    FS_DirectoryEntry CurrentEntry;
    while (R_SUCCEEDED(FSDIR_Read(m_DirectoryHandle, &EntriesRead, 1, &CurrentEntry)) && EntriesRead == 1)
    {
        m_DirectoryList.push_back(CurrentEntry);
    }
    Directory::Close();
    // This will sort Folder->Alphabetical. This spits tons of warnings from GCC about versions, but it works fine so I'm not going to worry about that.
    std::sort(m_DirectoryList.begin(), m_DirectoryList.end(), CompareEntries);
    m_WasOpened = true;
}

bool FsLib::Directory::IsOpen(void) const
{
    return m_WasOpened;
}

uint32_t FsLib::Directory::GetEntryCount(void) const
{
    return m_DirectoryList.size();
}

bool FsLib::Directory::EntryAtIsDirectory(int Index) const
{
    if (Index < 0 || Index >= static_cast<int>(m_DirectoryList.size()))
    {
        return false;
    }
    return (m_DirectoryList[Index].attributes & FS_ATTRIBUTE_DIRECTORY);
}

std::u16string_view FsLib::Directory::GetEntryAt(int Index) const
{
    if (Index < 0 || Index >= static_cast<int>(m_DirectoryList.size()))
    {
        // I'm hoping this works the way I want and string_view.empty is true.
        return std::u16string_view(u"");
    }
    return std::u16string_view(reinterpret_cast<const char16_t *>(m_DirectoryList.at(Index).name));
}

const char16_t *FsLib::Directory::operator[](int Index) const
{
    if (Index < 0 || Index >= static_cast<int>(m_DirectoryList.size()))
    {
        return nullptr;
    }
    return reinterpret_cast<const char16_t *>(m_DirectoryList.at(Index).name);
}

bool FsLib::Directory::Close(void)
{
    Result FsError = FSDIR_Close(m_DirectoryHandle);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error closing directory handle: 0x%08X.", FsError);
        return false;
    }
    return true;
}
