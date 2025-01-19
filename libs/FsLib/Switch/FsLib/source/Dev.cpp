#include "Dev.hpp"
#include "File.hpp"
#include "FileFunctions.hpp"
#include <fcntl.h>
#include <string_view>
#include <switch.h>
#include <sys/iosupport.h>
#include <unordered_map>

/*
    This file is a mess, but it kind of has to be :(
*/

// Declarations.
extern "C"
{
    static int FsLibDevOpen(struct _reent *Reent, void *FileID, const char *Path, int Flags, int Mode);
    static int FsLibDevClose(struct _reent *Reent, void *FileID);
    static ssize_t FsLibDevWrite(struct _reent *Reent, void *FileID, const char *Buffer, size_t BufferSize);
    static ssize_t FsLibDevRead(struct _reent *Reent, void *FileID, char *Buffer, size_t BufferSize);
    static off_t FsLibDevSeek(struct _reent *Reent, void *FileID, off_t Position, int Direction);
}

namespace
{
    // Map of files paired with their ID.
    std::unordered_map<int, FsLib::File> s_FileMap;

    // This is how we get stdio calls to the sdmc and redirect them to FsLib files instead.
    constexpr devoptab_t s_SDMCDevOp = {.name = "sdmc",
                                        .structSize = sizeof(int),
                                        .open_r = FsLibDevOpen,
                                        .close_r = FsLibDevClose,
                                        .write_r = FsLibDevWrite,
                                        .read_r = FsLibDevRead,
                                        .seek_r = FsLibDevSeek};
} // namespace

bool FsLib::Dev::InitializeSDMC(void)
{
    // This should kill fs_dev.
    fsdevUnmountAll();
    // Add my own SD device to newlib.
    if (AddDevice(&s_SDMCDevOp) < 0)
    {
        return false;
    }
    return true;
}

static inline bool FileIsValid(int FileID)
{
    if (s_FileMap.find(FileID) == s_FileMap.end())
    {
        return false;
    }
    return true;
}

extern "C"
{
    static int FsLibDevOpen(struct _reent *Reent, void *FileID, const char *Path, int Flags, int Mode)
    {
        // This is to keep track of which file we're on.
        static int CurrentFileID = 0;

        // Flags used to open fill.
        uint32_t OpenFlags = 0;

        // This is so we don't constantly construct and deconstruct paths when testing.
        FsLib::Path FilePath = Path;
        if (!FilePath.IsValid())
        {
            Reent->_errno = ENOENT;
            return -1;
        }

        switch (Flags & O_ACCMODE)
        {
            case O_RDONLY:
            {
                OpenFlags = FsOpenMode_Read;
            }
            break;

            case O_WRONLY:
            {
                OpenFlags = FsOpenMode_Write;
            }
            break;

            case O_RDWR:
            {
                OpenFlags = FsOpenMode_Read | FsOpenMode_Write;
            }
            break;

            default:
            {
                Reent->_errno = EINVAL;
                return -1;
            }
            break;
        }

        if (Flags & O_APPEND && !FsLib::FileExists(FilePath))
        {
            OpenFlags |= FsOpenMode_Create;
        }
        else if (Flags & O_APPEND)
        {
            OpenFlags |= FsOpenMode_Append;
        }
        else if (Flags & O_CREAT)
        {
            OpenFlags |= FsOpenMode_Create;
        }
        // Increase file id and assign incoming file ID to it.
        int CurrentFile = CurrentFileID++;
        *reinterpret_cast<int *>(FileID) = CurrentFile;

        // Try to open it.
        s_FileMap[CurrentFile].Open(FilePath, OpenFlags);
        if (!s_FileMap[CurrentFile].IsOpen())
        {
            s_FileMap.erase(CurrentFile);
            return -1;
        }
        return 0;
    }

    static int FsLibDevClose(struct _reent *Reent, void *FileID)
    {
        int TargetFileID = *reinterpret_cast<int *>(FileID);
        if (!FileIsValid(TargetFileID))
        {
            Reent->_errno = EBADF;
            return -1;
        }
        s_FileMap.erase(TargetFileID);
        return 0;
    }

    static ssize_t FsLibDevWrite(struct _reent *Reent, void *FileID, const char *Buffer, size_t BufferSize)
    {
        int TargetFileID = *reinterpret_cast<int *>(FileID);
        if (!FileIsValid(TargetFileID))
        {
            Reent->_errno = EBADF;
            return -1;
        }
        return s_FileMap[TargetFileID].Write(Buffer, BufferSize);
    }

    static ssize_t FsLibDevRead(struct _reent *Reent, void *FileID, char *Buffer, size_t BufferSize)
    {
        int TargetFileID = *reinterpret_cast<int *>(FileID);
        if (!FileIsValid(TargetFileID))
        {
            Reent->_errno = EBADF;
            return -1;
        }
        return s_FileMap[TargetFileID].Read(Buffer, BufferSize);
    }

    static ssize_t FsLibDevSeek(struct _reent *Reent, void *FileID, off_t Offset, int Direction)
    {
        int TargetFileID = *reinterpret_cast<int *>(FileID);
        if (!FileIsValid(TargetFileID))
        {
            Reent->_errno = EBADF;
            return -1;
        }

        switch (Direction)
        {
            case SEEK_SET:
            {
                s_FileMap[TargetFileID].Seek(Offset, FsLib::Stream::Beginning);
            }
            break;

            case SEEK_CUR:
            {
                s_FileMap[TargetFileID].Seek(Offset, FsLib::Stream::Current);
            }
            break;

            case SEEK_END:
            {
                s_FileMap[TargetFileID].Seek(Offset, FsLib::Stream::End);
            }
            break;
        }
        return s_FileMap[TargetFileID].Tell();
    }
}
