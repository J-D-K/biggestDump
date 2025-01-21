#include "io.hpp"
#include "console.hpp"
#include "logger.hpp"
#include "strings.hpp"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

namespace
{
    constexpr size_t FILE_BUFFER_SIZE = 0x600000;
}

// To do: Just have a single thread that uses std::apply with a tuple to stop spawning and joining threads.
void readThreadFunction(fslib::File &inputFile,
                        std::mutex &bufferMutex,
                        std::condition_variable &bufferCondition,
                        bool &bufferIsReady,
                        std::shared_ptr<unsigned char[]> sharedBuffer,
                        uint32_t &readSize)
{
    int64_t fileSize = inputFile.getSize();
    for (int64_t i = 0; i < fileSize;)
    {
        // This thread will spawn before the other thread writes.
        // Read into sharedBuffer
        readSize = inputFile.read(sharedBuffer.get(), FILE_BUFFER_SIZE);

        // Can't forget this or this will loop forever. Don't ask me how I know.
        i += readSize;

        // Signal to other thread that read happened and stuff.
        bufferIsReady = true;
        bufferCondition.notify_one();

        // Wait for the other thread to copy the contents before reading again.
        std::unique_lock<std::mutex> bufferLock(bufferMutex);
        bufferCondition.wait(bufferLock, [&bufferIsReady]() { return bufferIsReady == false; });
        // Mutex will release immediately. This works fine. I promise.
    }
}

static void copyFile(const fslib::Path &source, const fslib::Path &destination)
{
    fslib::File sourceFile(source, FsOpenMode_Read);
    fslib::File destinationFile(destination, FsOpenMode_Create | FsOpenMode_Write, sourceFile.getSize());
    if (!sourceFile.isOpen() || !destinationFile.isOpen())
    {
        Console::printf("*%s*\n", fslib::getErrorString());
        return;
    }
    // Print string to console.
    Console::printf(strings::getByName(strings::names::COPYING_FILE), source.cString());

    // Stuff for threaded copying.
    bool bufferIsReady = false;
    std::condition_variable bufferCondition;
    std::mutex bufferMutex;
    std::shared_ptr<unsigned char[]> sharedBuffer(new unsigned char[FILE_BUFFER_SIZE]);
    uint32_t readSize = 0;

    // This thread has its own local buffer to copy to so it doesn't hold up the read thread.
    std::unique_ptr<unsigned char[]> localBuffer(new unsigned char[FILE_BUFFER_SIZE]);

    // Grab source size.
    size_t fileSize = sourceFile.getSize();

    // Spawn read thread.
    std::thread readThread(readThreadFunction,
                           std::ref(sourceFile),
                           std::ref(bufferMutex),
                           std::ref(bufferCondition),
                           std::ref(bufferIsReady),
                           sharedBuffer,
                           std::ref(readSize));

    for (size_t i = 0; i < fileSize;)
    {
        // This thread needs this saved.
        uint32_t readCount = 0;
        // Wait for buffer signal.
        {
            // Scoped so this releases asap.
            std::unique_lock<std::mutex> bufferLock(bufferMutex);
            bufferCondition.wait(bufferLock, [&bufferIsReady]() { return bufferIsReady; });

            // Grab read count and copy shared to local.
            readCount = readSize;
            std::memcpy(localBuffer.get(), sharedBuffer.get(), readCount);

            // Signal other thread it's good to go.
            bufferIsReady = false;
            bufferCondition.notify_one();
        }
        // Write local to file.
        destinationFile.write(localBuffer.get(), readCount);
        // Update loop
        i += readCount;
    }
    // Join read thread.
    readThread.join();
    // Print that you won the game.
    Console::printf(strings::getByName(strings::names::DONE));
}

void copyDirectory(const fslib::Path &source, const fslib::Path &destination)
{
    fslib::Directory sourceDir(source);
    if (!sourceDir.isOpen())
    {
        logger::log(fslib::getErrorString());
        return;
    }

    for (int64_t i = 0; i < sourceDir.getCount(); i++)
    {
        if (sourceDir.isDirectory(i))
        {
            fslib::Path newSource = source / sourceDir[i];
            fslib::Path newDestination = destination / sourceDir[i];
            if (!fslib::createDirectory(newDestination))
            {
                continue;
            }
            copyDirectory(newSource, newDestination);
        }
        else
        {
            fslib::Path fullSource = source / sourceDir[i];
            fslib::Path fullDestination = destination / sourceDir[i];
            copyFile(fullSource, fullDestination);
        }
    }
}
