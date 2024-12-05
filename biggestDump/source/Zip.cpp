#include "Zip.hpp"
#include "Console.hpp"
#include "Strings.hpp"
#include <cstring>
#include <ctime>
#include <memory>
#include <minizip/zip.h>

namespace
{
    // biggestDump's file transfers aren't threaded or this would be larger.
    constexpr size_t FILE_TRANSFER_BUFFER_SIZE = 0x100000;
    // This is the error string so I don't actually have to type it over and over.
    const char *ERROR_STRING_TEMPLATE = "\t\t\t*%s*\n";
} // namespace

static void CopyFile(const FsLib::Path &FilePath, zipFile Zip)
{
    std::time_t Timer;
    std::time(&Timer);
    std::tm *LocalTime = std::localtime(&Timer);

    zip_fileinfo FileInfo = {.tmz_date = {.tm_sec = LocalTime->tm_sec,
                                          .tm_hour = LocalTime->tm_hour,
                                          .tm_mday = LocalTime->tm_mday,
                                          .tm_mon = LocalTime->tm_mon,
                                          .tm_year = LocalTime->tm_year + 1900},
                             .dosDate = 0,
                             .internal_fa = 0,
                             .external_fa = 0};

    int ZipError = zipOpenNewFileInZip64(Zip, FilePath.GetPath() + 1, &FileInfo, NULL, 0, NULL, 0, NULL, 0, 0, 0);
    if (ZipError != ZIP_OK)
    {
        Console::Printf(ERROR_STRING_TEMPLATE, "Error opening file in ZIP.");
        return;
    }

    FsLib::File SourceFile(FilePath, FsOpenMode_Read);
    if (!SourceFile.IsOpen())
    {
        Console::Printf(ERROR_STRING_TEMPLATE, FsLib::GetErrorString());
        return;
    }

    Console::Printf(Strings::GetByName(Strings::Names::CopyingFileZip), FilePath.CString());
    std::unique_ptr<unsigned char[]> FileBuffer(new unsigned char[FILE_TRANSFER_BUFFER_SIZE]);
    for (int64_t i = 0; i < SourceFile.GetSize();)
    {
        size_t BytesRead = SourceFile.Read(FileBuffer.get(), FILE_TRANSFER_BUFFER_SIZE);
        if (BytesRead <= 0)
        {
            Console::Printf(ERROR_STRING_TEMPLATE, FsLib::GetErrorString());
            return;
        }

        ZipError = zipWriteInFileInZip(Zip, FileBuffer.get(), BytesRead);
        if (ZipError != ZIP_OK)
        {
            Console::Printf(ERROR_STRING_TEMPLATE, "Error writing to file in ZIP.");
            return;
        }
        i += BytesRead;
    }
    zipCloseFileInZip(Zip);
    Console::Printf(Strings::GetByName(Strings::Names::Done));
}

static void CopyDirectory(const FsLib::Path &SourcePath, zipFile Zip)
{
    FsLib::Directory Dir(SourcePath);
    if (!Dir.IsOpen())
    {
        Console::Printf(ERROR_STRING_TEMPLATE, FsLib::GetErrorString());
        return;
    }

    for (int64_t i = 0; i < Dir.GetEntryCount(); i++)
    {
        FsLib::Path NewSource = SourcePath / Dir[i];
        if (Dir.EntryAtIsDirectory(i))
        {
            CopyDirectory(NewSource, Zip);
        }
        else
        {
            CopyFile(NewSource, Zip);
        }
    }
}

void CopyDirectoryToZip(const FsLib::Path &DirectoryPath, const char *ZipPath)
{
    zipFile TargetZip = zipOpen64(ZipPath, APPEND_STATUS_CREATE);
    if (!TargetZip)
    {
        Console::Printf("Error opening %s for writing!\n", ZipPath);
        return;
    }
    CopyDirectory(DirectoryPath, TargetZip);
    zipClose(TargetZip, NULL);
}
