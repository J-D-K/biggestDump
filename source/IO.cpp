#include "IO.hpp"
#include "Console.hpp"
#include "Strings.hpp"
#include <memory>

namespace
{
    constexpr int FILE_BUFFER_SIZE = 0x100000;
}

void CopyFile(const FsLib::Path &SourcePath, const FsLib::Path &DestinationPath)
{
    FsLib::File SourceFile(SourcePath, FsOpenMode_Read);
    FsLib::File DestinationFile(DestinationPath, FsOpenMode_Create | FsOpenMode_Write, SourceFile.GetSize());

    if (!SourceFile.IsOpen() || !DestinationFile.IsOpen())
    {
        Console::Printf("*%s*\n", FsLib::GetErrorString());
        return;
    }

    Console::Printf(Strings::GetByName(Strings::Names::CopyingFile), SourcePath.CString());
    std::unique_ptr<unsigned char[]> FileBuffer(new unsigned char[FILE_BUFFER_SIZE]);
    for (int64_t i = 0; i < SourceFile.GetSize();)
    {
        size_t BytesRead = SourceFile.Read(FileBuffer.get(), FILE_BUFFER_SIZE);
        if (BytesRead == 0)
        {
            Console::Printf("*%s*\n", FsLib::GetErrorString());
            break;
        }

        if (DestinationFile.Write(FileBuffer.get(), BytesRead) == 0)
        {
            Console::Printf("*%s*\n", FsLib::GetErrorString());
            break;
        }
        i += BytesRead;
    }
    Console::Printf(Strings::GetByName(Strings::Names::Done));
}

void CopyDirectoryToDirectory(const FsLib::Path &Source, const FsLib::Path &Destination)
{
    FsLib::Directory Dir(Source);
    if (!Dir.IsOpen())
    {
        Console::Printf("*%s*\n", FsLib::GetErrorString());
        return;
    }

    for (int64_t i = 0; i < Dir.GetEntryCount(); i++)
    {
        if (Dir.EntryAtIsDirectory(i))
        {
            FsLib::Path NewSource = Source / Dir[i];
            FsLib::Path NewDestination = Destination / Dir[i];
            if (!FsLib::CreateDirectory(NewDestination))
            {
                Console::Printf("*%s*\n", FsLib::GetErrorString());
                continue;
            }

            CopyDirectoryToDirectory(NewSource, NewDestination);
        }
        else
        {
            FsLib::Path SourcePath = Source / Dir[i];
            FsLib::Path DestinationPath = Destination / Dir[i];

            CopyFile(SourcePath, DestinationPath);
        }
    }
}
