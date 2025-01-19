#pragma once
#include "Path.hpp"
#include <memory>
#include <switch.h>

namespace FsLib
{
    /// @brief Class for opening and reading entries from directories.
    class Directory
    {
        public:
            /// @brief Default constructor for Directory.
            Directory(void) = default;

            /// @brief Attempts to open Directory path and read all entries. IsOpen can be used to check if this was successful.
            /// @param DirectoryPath Path to directory.
            Directory(const FsLib::Path &DirectoryPath);

            /// @brief Attempts to open Directory path and read all entries. IsOpen can be used to check if this was successful.
            /// @param DirectoryPath Path to directory.
            void Open(const FsLib::Path &DirectoryPath);

            /// @brief Returns if directory was successfully opened.
            /// @return True if it was. False if it wasn't.
            bool IsOpen(void) const;

            /// @brief Returns total number of entries read from directory.
            /// @return Total numbers of entries read from directory.
            int64_t GetEntryCount(void) const;

            /// @brief Returns the size of the entry at Index.
            /// @param Index Index of entry.
            /// @return Size of entry. 0 if Index is out of bounds.
            int64_t GetEntrySizeAt(int Index) const;

            /// @brief Returns the name of the entry at Index.
            /// @param Index Index of entry.
            /// @return Name of the entry. nullptr if Index is out of bounds.
            const char *GetEntryAt(int Index) const;

            /// @brief Returns whether or not the entry at Index is a directory.
            /// @param Index Index of entry.
            /// @return True if the item is a directory. False if it is not or out of bounds.
            bool EntryAtIsDirectory(int Index) const;

            /// @brief Returns entry name at Index.
            /// @param Index Index of entry.
            /// @return Entry's name. If out of bounds, nullptr.
            const char *operator[](int Index) const;

        private:
            // Directory Handle/service.
            FsDir m_DirectoryHandle;
            // Number of entries in directory
            int64_t m_EntryCount = 0;
            // Entries in directory.
            std::unique_ptr<FsDirectoryEntry[]> m_DirectoryList;
            // Whether or not directory was opened and read.
            bool m_WasRead = false;

            /// @brief Private: Closes directory handle. Directory is never truly kept open. This is not needed outside of Directory.
            void Close(void);
    };
} // namespace FsLib
