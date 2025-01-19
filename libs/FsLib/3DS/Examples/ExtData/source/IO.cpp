#include "IO.hpp"
#include "String.hpp"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

// This is the size of the buffer used to copy files from ExtData. This is 1MB because this is threaded and I'm not sure much would be gained
// from raising it. You can change it to see what it does.
static constexpr size_t FILE_BUFFER_SIZE = 0x40000;

// I'm too lazy to type unsigned char over and over.
using Byte = unsigned char;

// This struct is shared between the main thread and the one writing the data after the main thread is done reading it.
typedef struct
{
        std::condition_variable BufferCondition;
        std::mutex BufferLock;
        bool BufferIsFull = false;
        std::vector<Byte> SharedBuffer;
        size_t FileSize = 0;
        size_t BytesRead = 0;
} SharedWriteData;

// This is the function that is going to be running in our thread.
void WriteThreadFunction(FsLib::File &FileOut, std::shared_ptr<SharedWriteData> SharedStruct)
{
    // This keeps track of how many bytes were written by this thread.
    size_t TotalBytesWritten = 0;
    // This is a local buffer this thread uses to copy the data to write it while the main one gets back to reading.
    std::vector<Byte> LocalBuffer;
    // This will keep the loop running as long as there's still file to read if you didn't know that already.
    while (TotalBytesWritten < SharedStruct->FileSize)
    {
        {
            // This is C++, but you can do this in C too. For stuff like this though, I find C++ better even if most people can't wrap their heads around it.
            // This will lock the mutex so this thread and the main one don't access the shared vector at the same time and corrupt it.
            std::unique_lock<std::mutex> SharedBufferLock(SharedStruct->BufferLock);
            // This will wait for the other thread's notification and check to make sure the buffer is full and ready to be copied then written.
            // This is what actually locks the mutex. I don't want to be insulting, but this uses a lambda expression. Just look it up if you don't understand what they are.
            SharedStruct->BufferCondition.wait(SharedBufferLock, [SharedStruct]() { return SharedStruct->BufferIsFull; });
            // Now the mutex is locked and only this thread can touch the shared buffer. We're going to copy it's data to LocalBuffer.
            // We need to assign it like this because the vector we're using always has a fixed size.
            LocalBuffer.assign(SharedStruct->SharedBuffer.begin(), SharedStruct->SharedBuffer.begin() + SharedStruct->BytesRead);
            // Now we clear out the shared buffer.
            SharedStruct->SharedBuffer.clear();
            // This will tell the main thread that it can start reading data again.
            SharedStruct->BufferIsFull = false;
            // This notifies the other thread that it can start reading again.
            SharedStruct->BufferCondition.notify_one();
            // Again, I don't want to be insulting, but this is scoped so now the unique_lock goes out of scope after this and releases the mutex.
        }
        // Since Output file returns the number of bytes written, we can just add that to this to update it.
        TotalBytesWritten += FileOut.Write(LocalBuffer.data(), LocalBuffer.size());
        // Clear the local Buffer for next chunk of file data.
        LocalBuffer.clear();
    }
}

void CopyFile(const FsLib::Path &Source, const FsLib::Path &Destination)
{
    // The 3DS's screen is really cramped but you wanna be able to see what's happening, I guess.
    printf("Copying %s to %s... ", ToUTF8(Source.CString()).c_str(), ToUTF8(Destination.CString()).c_str());
    // Open the source and destination files.
    // Source/Reading
    FsLib::File SourceFile(Source, FS_OPEN_READ);
    // Writing. This uses an overload of the OutputFile class that allows you to specify the size of the file when it's created. This is
    // actually required for write to ExtData.
    FsLib::File DestinationFile(Destination, FS_OPEN_CREATE | FS_OPEN_WRITE, SourceFile.GetSize());
    if (!SourceFile.IsOpen() || !DestinationFile.IsOpen())
    {
        printf("FsLib ERROR: %s", FsLib::GetErrorString());
        return;
    }

    // This is a newer-ish C++ thing called a shared_ptr. It allocates memory like new or malloc, but you don't have to worry about freeing it,
    // but it does have it's draw backs. This struct is going to allow the threads to coordinate reading and writing.
    std::shared_ptr<SharedWriteData> SharedStruct = std::make_shared<SharedWriteData>();
    // We need to grab the file's size for since the other thread has no access to the source.
    // Obviously, the buffer is empty by default, so we don't have to worry about that.
    SharedStruct->FileSize = SourceFile.GetSize();
    // We're going to resize the vector so it can be read into.
    SharedStruct->SharedBuffer.resize(FILE_BUFFER_SIZE);

    // This will spawn our file writing thread.
    std::thread WritingThread(WriteThreadFunction, std::ref(DestinationFile), SharedStruct);

    // We'll keep track of the bytes read with this.
    size_t TotalBytesRead = 0;
    // Same as the WriteThreadFunction
    while (TotalBytesRead < SharedStruct->FileSize)
    {
        // This is like above, but backwards. It looks weird, but it works, I promise. It's either this or have a third buffer for this thread wasting RAM.
        // Read into the shared vector. The other thread needs to know how much was actually read because the vector is a fixed size.
        SharedStruct->BytesRead = SourceFile.Read(SharedStruct->SharedBuffer.data(), FILE_BUFFER_SIZE);
        // Both threads need to know this for this to work.
        TotalBytesRead += SharedStruct->BytesRead;
        // Signal to the other thread it's ready to go.
        SharedStruct->BufferIsFull = true;
        SharedStruct->BufferCondition.notify_one();
        {
            // We're just going to make this thread wait for the other thread to copy the data and signal back the buffer is empty.
            std::unique_lock<std::mutex> SharedBufferLock(SharedStruct->BufferLock);
            SharedStruct->BufferCondition.wait(SharedBufferLock, [SharedStruct]() { return SharedStruct->BufferIsFull == false; });
        }
        // The mutex gets released right away here. There's no point in having locked down since the other thread is done with it. I'm sure
        // some people will tell me this is wrong, but meh.
    }
    // Wait for the other thread to finish all writing and close.
    WritingThread.join();
    // And we're done. Since nothing we used needed new or malloc, everything will take care of itself.
    printf("Done!\n");
}

void CopyDirectoryTo(const FsLib::Path &Source, const FsLib::Path &Destination)
{
    FsLib::Directory SourceDir(Source);
    if (!SourceDir.IsOpen())
    {
        return;
    }

    for (unsigned int i = 0; i < SourceDir.GetEntryCount(); i++)
    {
        if (SourceDir.EntryAtIsDirectory(i))
        {
            // New Source and Destination directory paths.
            FsLib::Path NewSource = Source / SourceDir.GetEntryAt(i);
            FsLib::Path NewDestination = Destination / SourceDir.GetEntryAt(i);
            // Make sure destination folder exists.
            if (!FsLib::DirectoryExists(NewDestination) && !FsLib::CreateDirectory(NewDestination))
            {
                printf("Failed to create %s: %s\n", ToUTF8(NewDestination.CString()).c_str(), FsLib::GetErrorString());
                // Continue the loop if we can't create it.
                continue;
            }
            CopyDirectoryTo(NewSource, NewDestination);
        }
        else
        {
            FsLib::Path SourceFile = Source / SourceDir.GetEntryAt(i);
            FsLib::Path DestinationFile = Destination / SourceDir.GetEntryAt(i);
            CopyFile(SourceFile, DestinationFile);
        }
    }
}
