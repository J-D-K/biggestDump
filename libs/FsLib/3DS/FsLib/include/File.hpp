#pragma once
#include "Path.hpp"
#include <3ds.h>
#include <cstdint>

// This is to make this easier.
static constexpr uint32_t FS_OPEN_APPEND = BIT(3);

namespace FsLib
{
    /// @brief Class for reading and writing to files.
    class File
    {
        public:
            /// @brief Default FsLib::File constructor.
            File(void) = default;

            /**
             * @brief Attempts to open FilePath with OpenFlags. FileSize is optional unless trying to create new Extra Data type files.
             * @param FilePath Path to target file.
             * @param OpenFlags Flags to use to open the file. Can be any flag from Ctrulib. FsLib provides a new FS_OPEN_APPEND if needed.
             * @param FileSize Optional. Size used to create the file. This is only needed when creating and writing to Extra Data type archives.
             */
            File(const FsLib::Path &FilePath, uint32_t OpenFlags, uint64_t FileSize = 0);

            /// @brief Automatically closes handle when freed or goes out of scope.
            ~File();

            /**
             * @brief Attempts to open FilePath with OpenFlags. FileSize is optional unless trying to create new Extra Data type files.
             * @param FilePath Path to target file.
             * @param OpenFlags Flags to use to open the file. Can be any flag from Ctrulib. FsLib provides a new FS_OPEN_APPEND if needed.
             * @param FileSize Optional. Size used to create the file. This is only needed when creating and writing to Extra Data type archives.
             */
            void Open(const FsLib::Path &FilePath, uint32_t OpenFlags, uint64_t FileSize = 0);

            /// @brief Can be used to manually close the file handle if needed.
            void Close(void);

            /// @brief Returns whether opening the file was successful or not.
            /// @return True on success. False on failure.
            bool IsOpen(void) const;

            /// @brief Returns the current offset of the file.
            /// @return Current file offset.
            uint64_t Tell(void) const;

            /// @brief Returns the size of the file.
            /// @return File's current size.
            uint64_t GetSize(void) const;

            /// @brief Returns whether or not the end of the file has been reached.
            /// @return True if the end is reached. False if not.
            bool EndOfFile(void) const;

            /// @brief Seeks to a position in file. Offsets are bounds checked.
            /// @param Offset Offset to seek to.
            /// @param Origin Position to seek from.
            void Seek(int64_t Offset, uint8_t Origin);

            /// @brief Attempts to read from file. Certain read errors are corrected for.
            /// @param Buffer Buffer to read into.
            /// @param ReadSize Size of the buffer to read into.
            /// @return Number of bytes read on success. 0 on complete failure. FsLib::GetError string can be used to get slightly more information.
            size_t Read(void *Buffer, size_t ReadSize);

            /// @brief Attempts to read a line AKA read bytes until `\n` or `\r` is hit.
            /// @param LineOut The string to write the line to.
            /// @return True on success. False on failure.
            bool ReadLine(std::string &LineOut);

            /// @brief Reads one char or byte from file.
            /// @return Byte read on success. -1 on failure.
            signed char GetCharacter(void);

            /// @brief Attempts to write Buffer to File. File is automatically resized to fit Buffer if needed.
            /// @param Buffer Buffer to write to file.
            /// @param WriteSize Size of Buffer
            /// @return Number of bytes written on success. 0 on complete failure.
            size_t Write(const void *Buffer, size_t WriteSize);

            /// @brief Attempts to write a formatted string to file.
            /// @param Format Format of string.
            /// @param Arguments
            /// @return True on success. False on failure.
            bool Writef(const char *Format, ...);

            /// @brief std style operators for quick, easy string writing.
            /// @param String String to write to file.
            /// @return Reference to current file.
            File &operator<<(const char *String);
            File &operator<<(const std::string &String);

            /// @brief Attempts to write a single char or byte to file.
            /// @param C Char or byte to write.
            /// @return True on success. False on failure.
            bool PutCharacter(char C);

            /// @brief Flushes the file.
            /// @return True on success. False on failure.
            bool Flush(void);

            /// @brief Used to seek from the beginning of the file.
            static constexpr uint8_t Beginning = 0;
            /// @brief Used to seek from the current offset of the file.
            static constexpr uint8_t Current = 1;
            /// @brief Used to seek from the end of the file.
            static constexpr uint8_t End = 2;

        protected:
            /// @brief Handle to file.
            Handle m_FileHandle;

            /// @brief Stores whether open was successful or not.
            bool m_IsOpen = false;

            /// @brief Stores flags passed to open.
            uint32_t m_Flags;

            /// @brief Store the current offset in the file and the size of the file.
            int64_t m_Offset, m_FileSize;

            /// @brief Private: Corrects if offset is out of bounds. Ex: m_Offset < 0 or m_Offset > m_FileSize
            void EnsureOffsetIsValid(void);

            /// @brief Attempts to resize a file if the buffer size is too large to fit in the remaining space.
            /// @param BufferSize Size of buffer to check.
            /// @return True on success. False on failure.
            bool ResizeIfNeeded(size_t BufferSize);

            /// @brief Returns whether or not the file is open for reading by checking m_Flags.
            /// @return True if it is. False if it isn't.
            inline bool IsOpenForReading(void) const
            {
                return m_Flags & FS_OPEN_READ;
            }

            /// @brief Returns whether or not the file is open for writing by checking m_Flags.
            /// @return True if it is. False if it isn't.
            inline bool IsOpenForWriting(void) const
            {
                return m_Flags & FS_OPEN_WRITE;
            }
    };
} // namespace FsLib
