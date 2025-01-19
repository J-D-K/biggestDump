#include "File.hpp"
#include "ErrorCommon.h"
#include "FileFunctions.hpp"
#include "FsLib.hpp"
#include "String.hpp"
#include <cstdarg>
#include <string>

namespace
{
    constexpr size_t VA_BUFFER_SIZE = 0x1000;
    const char *ERROR_NOT_OPEN_FOR_READING = "Error: File not open for reading.";
    const char *ERROR_NOT_OPEN_FOR_WRITING = "Error: File not open for writing.";
} // namespace

extern std::string g_FsLibErrorString;

FsLib::File::File(const FsLib::Path &FilePath, uint32_t OpenFlags, int64_t FileSize)
{
    File::Open(FilePath, OpenFlags, FileSize);
}

FsLib::File::~File()
{
    File::Close();
}

void FsLib::File::Open(const FsLib::Path &FilePath, uint32_t OpenFlags, int64_t FileSize)
{
    // Just in case this is reused.
    File::Close();

    // Normally I wouldn't do this, but...
    if ((OpenFlags & FsOpenMode_Append) && !(OpenFlags & FsOpenMode_Write))
    {
        OpenFlags |= FsOpenMode_Write;
    }

    if (!FilePath.IsValid())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return;
    }

    FsFileSystem *FileSystem;
    if (!FsLib::GetFileSystemByDeviceName(FilePath.GetDeviceName(), &FileSystem))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return;
    }

    if ((OpenFlags & FsOpenMode_Create) && FsLib::FileExists(FilePath) && !FsLib::DeleteFile(FilePath))
    {
        // Previous calls should set error. Let's not worry about it.
        return;
    }

    if ((OpenFlags & FsOpenMode_Create) && !FsLib::CreateFile(FilePath, FileSize))
    {
        return;
    }

    // We need to remove FsLib's added flag.
    OpenFlags &= ~FsOpenMode_Create;

    Result FsError = fsFsOpenFile(FileSystem, FilePath.GetPath(), OpenFlags, &m_FileHandle);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening file: 0x%X.", FsError);
        return;
    }

    FsError = fsFileGetSize(&m_FileHandle, &m_StreamSize);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error getting file's size: 0x%X.", FsError);
        return;
    }
    m_OpenFlags = OpenFlags;
    m_Offset = (m_OpenFlags & FsOpenMode_Append) ? m_StreamSize : 0;
    m_IsOpen = true;
}

void FsLib::File::Close(void)
{
    if (m_IsOpen)
    {
        fsFileClose(&m_FileHandle);
        m_IsOpen = false;
    }
}

bool FsLib::File::IsOpen(void) const
{
    return m_IsOpen;
}

ssize_t FsLib::File::Read(void *Buffer, size_t BufferSize)
{
    if (!m_IsOpen || !File::IsOpenForReading())
    {
        g_FsLibErrorString = ERROR_NOT_OPEN_FOR_READING;
        return 0;
    }

    uint64_t BytesRead = 0;
    Result FsError = fsFileRead(&m_FileHandle, m_Offset, Buffer, BufferSize, FsReadOption_None, &BytesRead);
    if (R_FAILED(FsError) || BytesRead > BufferSize) // Carrying over that last one from 3DS...
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error reading from file: 0x%X.", FsError);
        // I don't think this is a problem on Switch, but just in case.
        BytesRead = m_Offset + static_cast<int64_t>(BufferSize) > m_StreamSize ? m_StreamSize - m_Offset : BufferSize;
    }
    m_Offset += BytesRead;
    return BytesRead;
}

bool FsLib::File::ReadLine(char *LineOut, size_t LineLength)
{
    if (!m_IsOpen || !File::IsOpenForReading())
    {
        g_FsLibErrorString = ERROR_NOT_OPEN_FOR_READING;
        return false;
    }

    signed char NextCharacter = 0x00;
    for (size_t i = 0; i < LineLength; i++)
    {
        if (Stream::EndOfStream() || (NextCharacter = File::GetCharacter()) == -1)
        {
            return false;
        }

        if (NextCharacter == '\n' || NextCharacter == '\r')
        {
            return true;
        }

        LineOut[i] = NextCharacter;
    }
    return false;
}

signed char FsLib::File::GetCharacter(void)
{
    if (!m_IsOpen || !File::IsOpenForReading())
    {
        g_FsLibErrorString = ERROR_NOT_OPEN_FOR_READING;
        return -1;
    }

    // I don't want to run through another function for this.
    char Character = 0x00;
    uint64_t BytesRead = 0;
    Result FsError = fsFileRead(&m_FileHandle, m_Offset++, &Character, 1, FsReadOption_None, &BytesRead);
    if (R_FAILED(FsError) || BytesRead == 0)
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error reading from file: 0x%X.", FsError);
        return -1;
    }
    return Character;
}

ssize_t FsLib::File::Write(const void *Buffer, size_t BufferSize)
{
    // To do: This error is wrong for the last condition, but for now.
    if (!m_IsOpen || !File::IsOpenForWriting() || !File::ResizeIfNeeded(BufferSize))
    {
        g_FsLibErrorString = ERROR_NOT_OPEN_FOR_WRITING;
        return 0;
    }

    Result FsError = fsFileWrite(&m_FileHandle, m_Offset, Buffer, BufferSize, FsWriteOption_None);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error writing to file: 0x%X.", FsError);
        return 0;
    }
    // There's no real way to verify this was successful on Switch.
    m_Offset += BufferSize;
    return BufferSize;
}

bool FsLib::File::Writef(const char *Format, ...)
{
    char VaBuffer[VA_BUFFER_SIZE] = {0};
    std::va_list VaList;
    va_start(VaList, Format);
    vsnprintf(VaBuffer, VA_BUFFER_SIZE, Format, VaList);
    va_end(VaList);

    return File::Write(VaBuffer, std::char_traits<char>::length(VaBuffer));
}

bool FsLib::File::PutCharacter(char C)
{
    // L o L
    if (!m_IsOpen || !File::IsOpenForWriting() || !File::ResizeIfNeeded(1))
    {
        g_FsLibErrorString = ERROR_NOT_OPEN_FOR_WRITING;
        return false;
    }

    // I'm not running this through another function for 1 byte.
    Result FsError = fsFileWrite(&m_FileHandle, m_Offset++, &C, 1, FsWriteOption_None);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error writing a single byte to file: 0x%X.", FsError);
        return false;
    }
    return true;
}

FsLib::File &FsLib::File::operator<<(const char *String)
{
    File::Write(String, std::char_traits<char>::length(String));
    return *this;
}

FsLib::File &FsLib::File::operator<<(const std::string &String)
{
    File::Write(String.c_str(), String.length());
    return *this;
}

bool FsLib::File::Flush(void)
{
    if (!m_IsOpen || !File::IsOpenForWriting())
    {
        g_FsLibErrorString = ERROR_NOT_OPEN_FOR_WRITING;
        return false;
    }

    Result FsError = fsFileFlush(&m_FileHandle);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error flushing file: 0x%X.", FsError);
        return false;
    }
    return true;
}

bool FsLib::File::ResizeIfNeeded(size_t BufferSize)
{
    size_t SpaceRemaining = m_StreamSize - m_Offset;

    // Resize isn't required. Everything will be fine.
    if (BufferSize < SpaceRemaining)
    {
        return true;
    }

    int64_t NewFileSize = m_Offset + BufferSize;
    Result FsError = fsFileSetSize(&m_FileHandle, NewFileSize);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error resizing file for writing: 0x%X.", FsError);
        return false;
    }
    m_StreamSize = NewFileSize;
    return true;
}
