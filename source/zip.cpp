#include "zip.hpp"
#include "console.hpp"
#include "strings.hpp"
#include <cstring>
#include <ctime>
#include <memory>
#include <minizip/zip.h>

namespace
{
    // biggestDump's file transfers aren't threaded or this would be larger.
    constexpr size_t FILE_TRANSFER_BUFFER_SIZE = 0x40000;
    // This is the error string so I don't actually have to type it over and over.
    const char *ERROR_STRING_TEMPLATE = "\t\t\t*%s*\n";
} // namespace

// Declarations here. Definitions later.
static void copyFile(const fslib::Path &filePath, zipFile zip);
static void copyDirectory(const fslib::Path &source, zipFile zip);

void copyDirectoryToZip(const fslib::Path &directoryPath, const char *zipPath)
{
    zipFile targetZip = zipOpen64(zipPath, APPEND_STATUS_CREATE);
    if (!targetZip)
    {
        Console::printf("Error opening \"%s\" for writing!\n", zipPath);
        return;
    }
    copyDirectory(directoryPath, targetZip);
    zipClose(targetZip, NULL);
}

static void copyFile(const fslib::Path &filePath, zipFile zip)
{
    std::time_t timer;
    std::time(&timer);
    std::tm *localTime = std::localtime(&timer);


    zip_fileinfo fileInfo = {.tmz_date = {.tm_sec = localTime->tm_sec,
                                          .tm_hour = localTime->tm_hour,
                                          .tm_mday = localTime->tm_mday,
                                          .tm_mon = localTime->tm_mon,
                                          .tm_year = localTime->tm_year + 1900},
                             .dosDate = 0,
                             .internal_fa = 0,
                             .external_fa = 0};

    int zipError = zipOpenNewFileInZip64(zip, filePath.getPath() + 1, &fileInfo, NULL, 0, NULL, 0, NULL, Z_NO_COMPRESSION, 0, 0);
    if (zipError != ZIP_OK)
    {
        Console::printf(ERROR_STRING_TEMPLATE, "Error opening file in ZIP!");
        return;
    }

    fslib::File sourceFile(filePath, FsOpenMode_Read);
    if (!sourceFile.isOpen())
    {
        Console::printf(ERROR_STRING_TEMPLATE, fslib::getErrorString());
        return;
    }

    // Print we're copying so people know we're copying.
    Console::printf(strings::getByName(strings::names::COPYING_FILE_ZIP), filePath.cString());
    std::unique_ptr<unsigned char[]> fileBuffer(new unsigned char[FILE_TRANSFER_BUFFER_SIZE]);
    for (int64_t i = 0; i < sourceFile.getSize();)
    {
        size_t bytesRead = sourceFile.read(fileBuffer.get(), FILE_TRANSFER_BUFFER_SIZE);
        if (bytesRead <= 0)
        {
            Console::printf(ERROR_STRING_TEMPLATE, fslib::getErrorString());
            return;
        }

        zipError = zipWriteInFileInZip(zip, fileBuffer.get(), bytesRead);
        if (zipError != ZIP_OK)
        {
            Console::printf(ERROR_STRING_TEMPLATE, "Error writing to file in ZIP.");
            return;
        }
        i += bytesRead;
    }
    zipCloseFileInZip(zip);
    Console::printf(strings::getByName(strings::names::DONE));
}

static void copyDirectory(const fslib::Path &source, zipFile zip)
{
    fslib::Directory sourceDir(source);
    if (!sourceDir.isOpen())
    {
        Console::printf(ERROR_STRING_TEMPLATE, fslib::getErrorString());
        return;
    }

    for (int64_t i = 0; i < sourceDir.getCount(); i++)
    {
        fslib::Path newSource = source / sourceDir[i];
        if (sourceDir.isDirectory(i))
        {
            copyDirectory(newSource, zip);
        }
        else
        {
            copyFile(newSource, zip);
        }
    }
}
