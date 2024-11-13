#include "IO.hpp"
#include "Console.hpp"
#include <memory>

namespace
{
    constexpr int FILE_BUFFER_SIZE = 0x100000;
}

void CopyFile(const FsLib::Path &SourcePath, const FsLib::Path &DestinationPath)
{
    FsLib::InputFile SourceFile(SourcePath);
    FsLib::OutputFile DestinationFile(DestinationPath, false);
    if (!SourceFile.IsOpen() || !DestinationFile.IsOpen())
    {
        Console::Get() << "*" << FsLib::GetErrorString() << "*" << "\n";
        return;
    }

    Console::Get() << "Copying " << SourcePath.CString() << " to SD..." << "\n";
    std::unique_ptr<unsigned char[]> FileBuffer(new unsigned char[FILE_BUFFER_SIZE]);
    for (int64_t i = 0; i < SourceFile.GetSize();)
    {
        size_t BytesRead = SourceFile.Read(FileBuffer.get(), FILE_BUFFER_SIZE);
        if (BytesRead == 0)
        {
            Console::Get() << "*" << FsLib::GetErrorString() << "*" << "\n";
            break;
        }

        if (DestinationFile.Write(FileBuffer.get(), BytesRead) == 0)
        {
            Console::Get() << "*" << FsLib::GetErrorString() << "*" << "\n";
            break;
        }
        i += BytesRead;
    }
}

void CopyDirectoryToDirectory(const FsLib::Path &Source, const FsLib::Path &Destination)
{
    FsLib::Directory Dir(Source);
    if (!Dir.IsOpen())
    {
        Console::Get() << FsLib::GetErrorString() << "\n";
        return;
    }

    for (int64_t i = 0; i < Dir.GetEntryCount(); i++)
    {
        if (Dir.EntryAtIsDirectory(i))
        {
            FsLib::Path NewSource = Source + Dir.GetEntryAt(i) + "/";
            FsLib::Path NewDestination = Destination + Dir.GetEntryAt(i) + "/";
            if (!FsLib::CreateDirectory(NewDestination.SubPath(NewDestination.GetLength() - 1)))
            {
                Console::Get() << "*" << FsLib::GetErrorString() << "*" << "\n";
                continue;
            }

            CopyDirectoryToDirectory(NewSource, NewDestination);
        }
        else
        {
            FsLib::Path SourcePath = Source + Dir.GetEntryAt(i);
            FsLib::Path DestinationPath = Destination + Dir.GetEntryAt(i);

            CopyFile(SourcePath, DestinationPath);
        }
    }
}
