#include "File.hpp"
#include "ErrorCommon.h"
#include "FsLib.hpp"
#include "String.hpp"
#include <cstdarg>

namespace
{
    static constexpr size_t VA_BUFFER_SIZE = 0x1000;
}

extern std::string g_FsLibErrorString;

FsLib::File::File(const FsLib::Path &FilePath, uint32_t OpenFlags, uint64_t FileSize)
{
    File::Open(FilePath, OpenFlags, FileSize);
}

FsLib::File::~File()
{
    File::Close();
}

void FsLib::File::Open(const FsLib::Path &FilePath, uint32_t OpenFlags, uint64_t FileSize)
{
    // Just to be sure.
    m_IsOpen = false;

    if (!FilePath.IsValid())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return;
    }

    FS_Archive Archive;
    if (!FsLib::GetArchiveByDeviceName(FilePath.GetDevice(), &Archive))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return;
    }

    // Need to save these.
    m_Flags = OpenFlags;

    if ((m_Flags & FS_OPEN_CREATE) && FsLib::FileExists(FilePath) && !FsLib::DeleteFile(FilePath))
    {
        return;
    }

    if ((m_Flags & FS_OPEN_CREATE) && !FsLib::CreateFile(FilePath, FileSize))
    {
        return;
    }

    // Need to remove this for Extra Data. I already took care of this part anyway.
    if (m_Flags & FS_OPEN_CREATE)
    {
        m_Flags &= ~FS_OPEN_CREATE;
    }

    Result FsError = FSUSER_OpenFile(&m_FileHandle, Archive, FilePath.GetPath(), m_Flags, 0);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening file: 0x%08X.", FsError);
        return;
    }

    FsError = FSFILE_GetSize(m_FileHandle, reinterpret_cast<uint64_t *>(&m_FileSize));
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error getting file size: 0x%08X.", FsError);
        return;
    }

    // I added FS_OPEN_APPEND to FsLib. This isn't normally part of ctrulib/3DS.
    m_Offset = m_Flags & FS_OPEN_APPEND ? m_FileSize : 0;

    // Should be good now.
    m_IsOpen = true;
}

void FsLib::File::Close(void)
{
    if (m_IsOpen)
    {
        FSFILE_Close(m_FileHandle);
    }
}

bool FsLib::File::IsOpen(void) const
{
    return m_IsOpen;
}

uint64_t FsLib::File::Tell(void) const
{
    return m_Offset;
}

uint64_t FsLib::File::GetSize(void) const
{
    return m_FileSize;
}

bool FsLib::File::EndOfFile(void) const
{
    return m_Offset >= m_FileSize;
}

void FsLib::File::Seek(int64_t Offset, uint8_t Origin)
{
    switch (Origin)
    {
        case FsLib::File::Beginning:
        {
            m_Offset = 0 + Offset;
        }
        break;

        case FsLib::File::Current:
        {
            m_Offset += Offset;
        }
        break;

        case FsLib::File::End:
        {
            m_Offset = m_FileSize + Offset;
        }
        break;
    }
    File::EnsureOffsetIsValid();
}

size_t FsLib::File::Read(void *Buffer, size_t ReadSize)
{
    if (!File::IsOpenForReading())
    {
        return 0;
    }

    uint32_t BytesRead = 0;
    Result FsError = FSFILE_Read(m_FileHandle, &BytesRead, static_cast<uint64_t>(m_Offset), Buffer, static_cast<uint32_t>(ReadSize));
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error reading from file: 0x%08X.", FsError);
        BytesRead = m_Offset + ReadSize > m_FileSize ? m_FileSize - m_Offset : ReadSize;
    }
    m_Offset += BytesRead;
    return BytesRead;
}

bool FsLib::File::ReadLine(std::string &LineOut)
{
    if (!File::IsOpenForReading())
    {
        return false;
    }

    LineOut.clear();

    char CurrentCharacter = 0x00;
    while ((CurrentCharacter = File::GetCharacter()) != -1)
    {
        if (CurrentCharacter == '\n' || CurrentCharacter == '\r')
        {
            return true;
        }
        LineOut += CurrentCharacter;
    }
    return false;
}

signed char FsLib::File::GetCharacter(void)
{
    if (!File::IsOpenForReading() || m_Offset >= m_FileSize)
    {
        return -1;
    }

    uint32_t BytesRead = 0;
    char CharacterRead = 0x00;
    Result FsError = FSFILE_Read(m_FileHandle, &BytesRead, m_Offset++, &CharacterRead, 1);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error reading byte from file: 0x%08X.", FsError);
        return 0x00;
    }
    return CharacterRead;
}

size_t FsLib::File::Write(const void *Buffer, size_t WriteSize)
{
    if (!File::IsOpenForWriting() || !File::ResizeIfNeeded(WriteSize))
    {
        return 0;
    }

    uint32_t BytesWritten = 0;
    Result FsError = FSFILE_Write(m_FileHandle, &BytesWritten, m_Offset, Buffer, WriteSize, 0);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error writing to file: 0x%08X.", FsError);
        return 0;
    }
    m_Offset += BytesWritten;
    return BytesWritten;
}

bool FsLib::File::Writef(const char *Format, ...)
{
    char VaBuffer[VA_BUFFER_SIZE] = {0};

    std::va_list VaList;
    va_start(VaList, Format);
    vsnprintf(VaBuffer, VA_BUFFER_SIZE, Format, VaList);
    va_end(VaList);

    size_t StringLength = std::char_traits<char>::length(VaBuffer);
    return File::Write(VaBuffer, StringLength) == StringLength;
}

FsLib::File &FsLib::File::operator<<(const char *String)
{
    size_t StringLength = std::char_traits<char>::length(String);
    File::Write(String, StringLength);
    return *this;
}

FsLib::File &FsLib::File::operator<<(const std::string &String)
{
    File::Write(String.c_str(), String.length());
    return *this;
}

bool FsLib::File::PutCharacter(char C)
{
    return File::Write(&C, 1) == 1;
}

bool FsLib::File::Flush(void)
{
    if (!File::IsOpenForWriting())
    {
        return false;
    }

    Result FsError = FSFILE_Flush(m_FileHandle);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error flushing file: 0x%08X.", FsError);
        return false;
    }
    return true;
}

void FsLib::File::EnsureOffsetIsValid(void)
{
    if (m_Offset < 0)
    {
        m_Offset = 0;
    }
    else if (m_Offset > m_FileSize)
    {
        m_Offset = m_FileSize;
    }
}

bool FsLib::File::ResizeIfNeeded(size_t BufferSize)
{
    size_t SpaceRemaining = m_FileSize - m_Offset;

    if (BufferSize < SpaceRemaining)
    {
        return true;
    }

    int64_t NewFileSize = m_Offset + BufferSize;
    Result FsError = FSFILE_SetSize(m_FileHandle, static_cast<uint64_t>(NewFileSize));
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error resizing file to fit buffer: 0x%08X.", FsError);
        return false;
    }
    m_FileSize = NewFileSize;
    return true;
}
