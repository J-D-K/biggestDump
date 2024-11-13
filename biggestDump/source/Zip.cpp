#include "Zip.hpp"
#include "Console.hpp"
#include <cstring>
#include <ctime>
#include <memory>
#include <minizip/zip.h>

namespace
{
    // biggestDump's file transfers aren't threaded or this would be larger.
    constexpr size_t FILE_TRANSFER_BUFFER_SIZE = 0x100000;
} // namespace

static void CopyFile(const FsLib::Path &FilePath, zipFile Zip)
{
    std::time_t Timer;
    std::time(&Timer);
    std::tm *LocalTime = localtime(&Timer);

    zip_fileinfo FileInfo = {.tmz_date = {.tm_sec = LocalTime->tm_sec,
                                          .tm_hour = LocalTime->tm_hour,
                                          .tm_mday = LocalTime->tm_mday,
                                          .tm_mon = LocalTime->tm_mon,
                                          .tm_year = LocalTime->tm_year + 1900}};

    int ZipError = zipOpenNewFileInZip64(Zip, FilePath.GetPath() + 1, &FileInfo, NULL, 0, NULL, 0, NULL, 0, 0, 1);
    if (ZipError != ZIP_OK)
    {
        Console::Get() << "\t\t*Error opening file in ZIP: " << std::to_string(ZipError) << "*" << "\n";
        return;
    }

    FsLib::InputFile SourceFile(FilePath);
    if (!SourceFile.IsOpen())
    {
        Console::Get() << "\t\t*" << FsLib::GetErrorString() << "*" << "\n";
        return;
    }

    Console::Get() << "Copying " << FilePath.CString() << " to zip..." << "\n";

    std::unique_ptr<unsigned char[]> FileBuffer(new unsigned char[FILE_TRANSFER_BUFFER_SIZE]);
    for (int64_t i = 0; i < SourceFile.GetSize();)
    {
        size_t BytesRead = SourceFile.Read(FileBuffer.get(), FILE_TRANSFER_BUFFER_SIZE);
        if (BytesRead <= 0)
        {
            Console::Get() << "\t\t*" << FsLib::GetErrorString() << "*" << "\n";
            return;
        }

        ZipError = zipWriteInFileInZip(Zip, FileBuffer.get(), BytesRead);
        if (ZipError != ZIP_OK)
        {
            Console::Get() << "\tError writing file in ZIP: " << std::to_string(ZipError) << "\n";
            return;
        }
        i += BytesRead;
    }
    zipCloseFileInZip(Zip);
}

static void CopyDirectory(const FsLib::Path &SourcePath, zipFile Zip)
{
    FsLib::Directory Dir(SourcePath);
    if (!Dir.IsOpen())
    {
        Console::Get() << "\t" << FsLib::GetErrorString() << "\n";
        return;
    }

    for (int64_t i = 0; i < Dir.GetEntryCount(); i++)
    {
        if (Dir.EntryAtIsDirectory(i))
        {
            FsLib::Path NewSourcePath = SourcePath + Dir.GetEntryAt(i) + "/";
            CopyDirectory(NewSourcePath, Zip);
        }
        else
        {
            FsLib::Path SourceFilePath = SourcePath + Dir.GetEntryAt(i);
            CopyFile(SourceFilePath, Zip);
        }
    }
}

void CopyDirectoryToZip(const FsLib::Path &DirectoryPath, const char *ZipPath)
{
    zipFile TargetZip = zipOpen64(ZipPath, 0);
    if (!TargetZip)
    {
        Console::Get() << "Error opening \"" << ZipPath << "\" for writing!" << "\n";
        return;
    }
    CopyDirectory(DirectoryPath, TargetZip);
    zipClose(TargetZip, NULL);
}
