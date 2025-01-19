#include "FsLib.hpp"
#include <fcntl.h>
#include <memory>
#include <string>
#include <sys/iosupport.h>
#include <unordered_map>

/*
    This is to help make FsLib work like a drop-in replacement for ctrulib's archive_dev. It's more of a compatibility layer to make libs work
    than a full replacement. It's basically a wrapper for newlib files -> FsLib files.
*/

// Declarations.
extern "C"
{
    static int FsLibDevOpen(struct _reent *Reent, void *FileWrapper, const char *FilePath, int Flags, int Mode);
    static int FsLibDevClose(struct _reent *Reent, void *FileWrapper);
    static ssize_t FsLibDevWrite(struct _reent *Reent, void *FileWrapper, const char *Buffer, size_t BufferSize);
    static ssize_t FsLibDevRead(struct _reent *Reent, void *FileWrapper, char *Buffer, size_t BufferSize);
    static off_t FsLibDevSeek(struct _reent *Reent, void *FileWrapper, off_t Offset, int From);
}

namespace
{
    constexpr devoptab_t s_SDMCdevoptab = {
        .name = "sdmc",
        .structSize = sizeof(int), // This is just the ID of the file. This is to pretty much bypass newlib as much as possible.
        .open_r = FsLibDevOpen,
        .close_r = FsLibDevClose,
        .write_r = FsLibDevWrite,
        .read_r = FsLibDevRead,
        .seek_r = FsLibDevSeek};

    // Map of open files.
    std::unordered_map<int, FsLib::File> s_FileMap;
} // namespace

// We only do this for the SDMC since archive_dev is a complete mess and I don't feel like writing a romfs thing.
bool FsLib::Dev::InitializeSDMC(void)
{
    if (AddDevice(&s_SDMCdevoptab) < 0)
    {
        return false;
    }
    return true;
}

extern "C"
{
    static inline bool FileIsValid(int FileID)
    {
        return s_FileMap.find(FileID) != s_FileMap.end();
    }

    int FsLibDevOpen(struct _reent *Reent, void *FileID, const char *FilePath, int Flags, int Mode)
    {
        // We're just going to ID these to tell them apart. No need for lots of messy code you can't even read and tons of arrays.
        static int CurrentFileID = 0;

        // Length of incoming path.
        size_t PathLength = std::char_traits<char>::length(FilePath);

        // Convert the path to UTF-16 so FsLib can work with it.
        FsLib::Path Path;
        {
            // Get path length and make sure it can hold the path.
            size_t BufferSize = (PathLength * 2) * sizeof(char16_t);
            char16_t PathBuffer[BufferSize] = {0};
            utf8_to_utf16(reinterpret_cast<uint16_t *>(PathBuffer), reinterpret_cast<const uint8_t *>(FilePath), BufferSize);
            Path = PathBuffer;
        }

        // I'm not rewriting this check.
        if (!Path.IsValid())
        {
            Reent->_errno = ENOENT;
            return -1;
        }

        uint32_t OpenFlags = 0;
        switch (Flags & O_ACCMODE)
        {
            case O_RDONLY:
            {
                OpenFlags = FS_OPEN_READ;
            }
            break;

            case O_WRONLY:
            {
                OpenFlags = FS_OPEN_WRITE;
            }
            break;

            case O_RDWR:
            {
                OpenFlags = FS_OPEN_READ | FS_OPEN_WRITE;
            }
            break;

            default:
            {
                Reent->_errno = EINVAL;
                return -1;
            }
            break;
        }

        // The first condition is a precaution.
        if (Flags & O_APPEND && !FsLib::FileExists(Path))
        {
            OpenFlags |= FS_OPEN_CREATE;
        }
        else if (Flags & O_APPEND)
        {
            OpenFlags |= FS_OPEN_APPEND;
        }
        else if (Flags & O_CREAT)
        {
            OpenFlags |= FS_OPEN_CREATE;
        }

        // Since we got here, increase ID count
        int CurrentFile = CurrentFileID++;
        *reinterpret_cast<int *>(FileID) = CurrentFile;

        // Set file in map.
        s_FileMap[CurrentFile].Open(Path, OpenFlags);
        if (!s_FileMap[CurrentFile].IsOpen())
        {
            // Erase from map and return error.
            s_FileMap.erase(CurrentFile);
            return -1;
        }
        // We're good.
        return 0;
    }

    int FsLibDevClose(struct _reent *Reent, void *FileID)
    {
        int ID = *reinterpret_cast<int *>(FileID);
        if (!FileIsValid(ID))
        {
            Reent->_errno = EBADF;
            return -1;
        }
        s_FileMap.erase(ID);
        return 0;
    }

    ssize_t FsLibDevWrite(struct _reent *Reent, void *FileID, const char *Buffer, size_t BufferSize)
    {
        int ID = *reinterpret_cast<int *>(FileID);
        if (!FileIsValid(ID))
        {
            Reent->_errno = EBADF;
            return -1;
        }
        // FsLib will catch this if it's not open for the right mode.
        return s_FileMap[ID].Write(Buffer, BufferSize);
    }

    ssize_t FsLibDevRead(struct _reent *Reent, void *FileID, char *Buffer, size_t BufferSize)
    {
        int ID = *reinterpret_cast<int *>(FileID);
        if (!FileIsValid(ID))
        {
            Reent->_errno = EBADF;
            return -1;
        }
        return s_FileMap[ID].Read(Buffer, BufferSize);
    }

    off_t FsLibDevSeek(struct _reent *Reent, void *FileID, off_t Offset, int From)
    {
        int ID = *reinterpret_cast<int *>(FileID);
        if (!FileIsValid(ID))
        {
            Reent->_errno = EBADF;
            return -1;
        }

        switch (From)
        {
            case SEEK_SET:
            {
                s_FileMap[ID].Seek(Offset, FsLib::File::Beginning);
            }
            break;

            case SEEK_CUR:
            {
                s_FileMap[ID].Seek(Offset, FsLib::File::Current);
            }
            break;

            case SEEK_END:
            {
                s_FileMap[ID].Seek(Offset, FsLib::File::End);
            }
            break;

            default:
            {
                return -1;
            }
            break;
        }
        return s_FileMap[ID].Tell();
    }
}
