#pragma once
#include "Path.hpp"
#include "Stream.hpp"
#include <switch.h>

/// @brief This is an added OpenMode flag for FsLib on Switch so File::Open knows for sure it's supposed to create the file.
static constexpr uint32_t FsOpenMode_Create = BIT(8);

namespace FsLib
{
    /// @brief Class for opening, reading, and writing to files.
    class File : public FsLib::Stream
    {
        public:
            /// @brief Default file constructor.
            File(void) = default;

            /**
             * @brief Attempts to open file at FilePath with OpenFlags. IsOpen can be used to check if this was successful.
             *
             * @param FilePath Path to file.
             * @param OpenFlags Flags from LibNX to use to open the file with.
             * @param FileSize Optional. Creates the file with a starting size defined.
             */
            File(const FsLib::Path &FilePath, uint32_t OpenFlags, int64_t FileSize = 0);

            /// @brief Closes file handle if it's still open.
            ~File();

            /// @brief Attempts to open file at FilePath with OpenFlags.
            /// @param FilePath Path to file.
            /// @param OpenFlags Flags from LibNX to use to open the file with.
            /// @param FileSize Optional. Creates the file with a starting size defined.
            void Open(const FsLib::Path &FilePath, uint32_t OpenFlags, int64_t FileSize = 0);

            /// @brief Closes file handle if needed. Destructor takes care of this for you normally.
            void Close(void);

            /// @brief Returns if file was successfully opened.
            /// @return
            bool IsOpen(void) const;

            /// @brief Attempts to read ReadSize bytes into Buffer from file.
            /// @param Buffer Buffer to write into.
            /// @param ReadSize Buffer's capacity.
            /// @return Number of bytes read.
            ssize_t Read(void *Buffer, size_t BufferSize);

            /// @brief Attempts to read a line from file until `\n` or `\r` is reached.
            /// @param LineOut Buffer to read line into.
            /// @param LineLength Size of line buffer.
            /// @return True on success. False on failure or line exceeding LineLength.
            bool ReadLine(char *LineOut, size_t LineLength);

            /// @brief Attempts to read a single character or byte from file.
            /// @return Byte read.
            signed char GetCharacter(void);

            /// @brief Attempts to write Buffer of BufferSize bytes to file.
            /// @param Buffer Buffer containing data.
            /// @param BufferSize Size of Buffer.
            /// @return Number of bytes (assumed to be) written to file. -1 on error.
            ssize_t Write(const void *Buffer, size_t BufferSize);

            /// @brief Attempts to write a formatted string to file.
            /// @param Format Format of string.
            /// @param Arguments Arguments.
            /// @return True on success. False on failure.
            bool Writef(const char *Format, ...);

            /// @brief Writes a single byte to file.
            /// @param C Byte to write.
            /// @return True on success. False on failure.
            bool PutCharacter(char C);

            /// @brief Operator for quick string writing.
            /// @param String String to write.
            /// @return Reference to file.
            File &operator<<(const char *String);

            /// @brief Operator for quick string writing.
            /// @param String String to write.
            /// @return Reference to file.
            File &operator<<(const std::string &String);

            /// @brief Flushes file.
            /// @return True on success. False on failure.
            bool Flush(void);

        private:
            /// @brief File handle.
            FsFile m_FileHandle;

            /// @brief Stores flags used to open file.
            uint32_t m_OpenFlags = 0;

            /// @brief Private: Resizes file if Buffer is too large to fit in remaining space.
            /// @param BufferSize Size of buffer.
            /// @return True on success. False on failure.
            bool ResizeIfNeeded(size_t BufferSize);

            /// @brief Private: Returns if file has flag set to read.
            /// @return True if flags are correct. False if not.
            inline bool IsOpenForReading(void) const
            {
                return (m_OpenFlags & FsOpenMode_Read);
            }

            /// @brief Private: Returns if file has flag set to write.
            /// @return True if flags are correct. False if not.
            inline bool IsOpenForWriting(void) const
            {
                return (m_OpenFlags & FsOpenMode_Write) || (m_OpenFlags & FsOpenMode_Append);
            }
    };
} // namespace FsLib
