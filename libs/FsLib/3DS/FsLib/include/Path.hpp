#pragma once
#include <3ds.h>
#include <cstdint>
#include <memory>
#include <string>

namespace FsLib
{
    /// @brief The maximum path length FsLib on 3DS supports.
    static constexpr size_t MAX_PATH = 0x301;

    /// @brief Class to make working with UTF-16 paths easier to manage.
    class Path
    {
        public:
            /// @brief Creates a new path for use with FsLib
            /// @param P String to assign from.
            Path(void) = default;

            /// @brief Creates a new path for use with FsLib
            /// @param P String to assign from.
            Path(const Path &P);

            /// @brief Creates a new path for use with FsLib
            /// @param P String to assign from.
            Path(const char16_t *P);

            /// @brief Creates a new path for use with FsLib
            /// @param P String to assign from.
            Path(const uint16_t *P);

            /// @brief Creates a new path for use with FsLib
            /// @param P String to assign from.
            Path(const std::u16string &P);

            /// @brief Creates a new path for use with FsLib
            /// @param P String to assign from.
            Path(std::u16string_view P);

            /// @brief Frees path buffer.
            ~Path();

            /// @brief Performs checks and returns if path is valid for use with FsLib.
            /// @return True if path is valid. False if it is not.
            bool IsValid(void) const;

            /// @brief Returns a sub-path ending at PathLength
            /// @param PathLength Length of subpath to return.
            /// @return Sub-path.
            Path SubPath(size_t PathLength) const;

            /// @brief Searches for first occurrence of Character in Path. Overload starts at Begin.
            /// @param Character Character to search.
            /// @return Position of Character or Path::NotFound on failure.
            size_t FindFirstOf(char16_t Character) const;
            size_t FindFirstOf(char16_t Character, size_t Begin) const;

            /// @brief Searches backwards to find last occurrence of Character in string. Overload starts at begin.
            /// @param Character Character to search for.
            /// @return Position of Character or Path::NotFound on failure.
            size_t FindLastOf(char16_t Character) const;
            size_t FindLastOf(char16_t Character, size_t Begin) const;

            /// @brief Returns the entire path as a C const char16_t* String
            /// @return Pointer to path string buffer.
            const char16_t *CString(void) const;

            /// @brief Returns the device as a UTF-16 u16string_view.
            /// @return Device string.
            std::u16string_view GetDevice(void) const;

            /// @brief Returns file name as u16string_view.
            /// @return File name
            std::u16string_view GetFileName(void) const;

            /// @brief Returns extension of path as u16string_view.
            /// @return Path's extension.
            std::u16string_view GetExtension(void) const;

            /// @brief Returns an FS_Path for use with 3DS FS functions.
            /// @return FS_Path
            FS_Path GetPath(void) const;

            /// @brief Returns length of the entire path string.
            /// @return Length of path string.
            size_t GetLength(void) const;

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param P Path to assign from
            /// @return Reference to current path.
            Path &operator=(const Path &P);

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param P Path to assign from
            /// @return Reference to current path.
            Path &operator=(const char16_t *P);

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param P Path to assign from
            /// @return Reference to current path.
            Path &operator=(const uint16_t *P);

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param P Path to assign from
            /// @return Reference to current path.
            Path &operator=(const std::u16string &P);

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param P Path to assign from
            /// @return Reference to current path.
            Path &operator=(std::u16string_view P);

            /// @brief Preferred appending operator. Adds / if needed between paths. Also trims slashes from input.
            /// @param P String to append.
            /// @return Reference to current Path
            Path &operator/=(const char16_t *P);

            /// @brief Preferred appending operator. Adds / if needed between paths. Also trims slashes from input.
            /// @param P String to append.
            /// @return Reference to current Path
            Path &operator/=(const uint16_t *P);

            /// @brief Preferred appending operator. Adds / if needed between paths. Also trims slashes from input.
            /// @param P String to append.
            /// @return Reference to current Path
            Path &operator/=(const std::u16string &P);

            /// @brief Preferred appending operator. Adds / if needed between paths. Also trims slashes from input.
            /// @param P String to append.
            /// @return Reference to current Path
            Path &operator/=(std::u16string_view P);

            /// @brief Unchecked appending operator. Input is not checked for validity of string appended.
            /// @param P String to append.
            /// @return Reference to current Path.
            Path &operator+=(const char16_t *P);

            /// @brief Unchecked appending operator. Input is not checked for validity of string appended.
            /// @param P String to append.
            /// @return Reference to current Path.
            Path &operator+=(const uint16_t *P);

            /// @brief Unchecked appending operator. Input is not checked for validity of string appended.
            /// @param P String to append.
            /// @return Reference to current Path.
            Path &operator+=(const std::u16string &P);

            /// @brief Unchecked appending operator. Input is not checked for validity of string appended.
            /// @param P String to append.
            /// @return Reference to current Path.
            Path &operator+=(std::u16string_view P);

            static constexpr uint16_t NotFound = -1;

        private:
            /// @brief Buffer 0x301 + Device length * sizeof(char16_t) bytes long containing path data.
            char16_t *m_Path = nullptr;

            /// @brief Pointer to where the end of the device in the path is located.
            const char16_t *m_DeviceEnd = nullptr;

            /// @brief Size of path buffer.
            uint16_t m_PathSize = 0;

            /// @brief Current length of the path.
            uint16_t m_PathLength = 0;

            /// @brief Allocates memory to hold path.
            /// @param PathSize Size of buffer for path.
            /// @return True on success. False on failure.
            /// @note The path is a raw pointer to allocated memory so copies can be made.
            bool AllocatePath(uint16_t PathSize);

            /// @brief Frees memory used for path buffer if it isn't nullptr.
            void FreePath(void);
    };

    /// @brief Concatenates a path to a string and returns a new one. Checks are performed and / is added if needed.
    /// @param Path1 BasePath
    /// @param Path2 Path to concatenate to Path1
    /// @return New path containing concatenated paths.
    FsLib::Path operator/(const FsLib::Path &Path1, const char16_t *Path2);

    /// @brief Concatenates a path to a string and returns a new one. Checks are performed and / is added if needed.
    /// @param Path1 BasePath
    /// @param Path2 Path to concatenate to Path1
    /// @return New path containing concatenated paths.
    FsLib::Path operator/(const FsLib::Path &Path1, const uint16_t *Path2);

    /// @brief Concatenates a path to a string and returns a new one. Checks are performed and / is added if needed.
    /// @param Path1 BasePath
    /// @param Path2 Path to concatenate to Path1
    /// @return New path containing concatenated paths.
    FsLib::Path operator/(const FsLib::Path &Path1, const std::u16string &Path2);

    /// @brief Concatenates a path to a string and returns a new one. Checks are performed and / is added if needed.
    /// @param Path1 BasePath
    /// @param Path2 Path to concatenate to Path1
    /// @return New path containing concatenated paths.
    FsLib::Path operator/(const FsLib::Path &Path1, std::u16string_view Path2);

    /// @brief Concatenates a path to a string and returns a new one. No checks are performed and Path2 is appended as-is.
    /// @param Path1 BasePath
    /// @param Path2 Path to concatenate to Path1
    /// @return New path containing concatenated paths.
    FsLib::Path operator+(const FsLib::Path &Path1, const char16_t *Path2);

    /// @brief Concatenates a path to a string and returns a new one. No checks are performed and Path2 is appended as-is.
    /// @param Path1 BasePath
    /// @param Path2 Path to concatenate to Path1
    /// @return New path containing concatenated paths.
    FsLib::Path operator+(const FsLib::Path &Path1, const uint16_t *Path2);

    /// @brief Concatenates a path to a string and returns a new one. No checks are performed and Path2 is appended as-is.
    /// @param Path1 BasePath
    /// @param Path2 Path to concatenate to Path1
    /// @return New path containing concatenated paths.
    FsLib::Path operator+(const FsLib::Path &Path1, const std::u16string &Path2);

    /// @brief Concatenates a path to a string and returns a new one. No checks are performed and Path2 is appended as-is.
    /// @param Path1 BasePath
    /// @param Path2 Path to concatenate to Path1
    /// @return New path containing concatenated paths.
    FsLib::Path operator+(const FsLib::Path &Path1, std::u16string_view Path2);
} // namespace FsLib
