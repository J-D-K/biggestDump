#pragma once
#include <filesystem>
#include <string>

namespace FsLib
{
    /// @brief Class to make working with the Switch's FS and it's odd rules much easier.
    class Path
    {
        public:
            /// @brief Default constructor for Path
            Path(void) = default;

            /// @brief Constructor for Path. Takes most standard C/C++ string types.
            /// @param P String to assign.
            Path(const Path &P);

            /// @brief Constructor for Path. Takes most standard C/C++ string types.
            /// @param P String to assign.
            Path(const char *P);

            /// @brief Constructor for Path. Takes most standard C/C++ string types.
            /// @param P String to assign.
            Path(const std::string &P);

            /// @brief Constructor for Path. Takes most standard C/C++ string types.
            /// @param P String to assign.
            Path(std::string_view P);

            /// @brief Constructor for Path. Takes most standard C/C++ string types.
            /// @param P String to assign.
            Path(const std::filesystem::path &P);

            /// @brief Frees memory used for path.
            ~Path();

            /**
             * @brief Returns whether or not path is valid for use with FsLib and Switch's FS.
             *
             * @return True if it is. False if it's not.
             * @note Based on four conditions:
                    1. The path was properly allocated and m_Path isn't nullptr.
                    2. There was a device found in the path.
                    3. The path length following the device is not empty.
                    4. The path has no illegal characters in it.
             */
            bool IsValid(void) const;

            /// @brief Returns a sub-path ending at PathLength.
            /// @param PathLength Length of sub-path to return.
            /// @return Sub-Path.
            Path SubPath(size_t PathLength) const;

            /// @brief Searches for the first occurrence of Character in path.
            /// @param Character Character to search for.
            /// @return Position of character in path. Path::NotFound if the character isn't found.
            size_t FindFirstOf(char Character) const;

            /// @brief Searches for the first occurrence of Character in path starting at Begin.
            /// @param Character Character to search for.
            /// @param Begin Postion to begin searching from.
            /// @return Position of character in path. Path::NotFound if the character wasn't found.
            size_t FindFirstOf(char Character, size_t Begin) const;

            /// @brief Searches backwards through path to find last occurrence of character in path.
            /// @param Character Character to search for.
            /// @return Position of character in path. Path::NotFound if the character wasn't found in path.
            size_t FindLastOf(char Character) const;

            /// @brief Searches backwards through path beginning at Begin to find last occurrence of character in path.
            /// @param Character Character to search for.
            /// @param Begin Position to "begin" at.
            /// @return Position of character in path. Path::NotFound if the character isn't found.
            size_t FindLastOf(char Character, size_t Begin) const;

            /// @brief Returns the entire path. Ex: sdmc:/Path/To/File.txt
            /// @return Entire path.
            const char *CString(void) const;

            /// @brief Returns the device at the beginning of the path for use with FsLib's internal functions. Ex: sdmc
            /// @return Device string.
            std::string_view GetDeviceName(void) const;

            /// @brief Returns the path after the device for use with Switch's FS functions. Ex: /Path/To/File.txt
            /// @return Filesystem path.
            const char *GetPath(void) const;

            /// @brief Returns full path length of the path buffer.
            /// @return Path length.
            size_t GetLength(void) const;

            /// @brief Assigns P to Path. Accepts most standard C/C++ string types.
            /// @param P Path to assign.
            /// @return Reference to path
            Path &operator=(const Path &P);

            /// @brief Assigns P to Path. Accepts most standard C/C++ string types.
            /// @param P String to assign.
            /// @return Reference to path
            Path &operator=(const char *P);

            /// @brief Assigns P to Path. Accepts most standard C/C++ string types.
            /// @param P String to assign.
            /// @return Reference to path
            Path &operator=(const std::string &P);

            /// @brief Assigns P to Path. Accepts most standard C/C++ string types.
            /// @param P String to assign.
            /// @return Reference to path
            Path &operator=(std::string_view P);

            /// @brief Assigns P to Path. Accepts most standard C/C++ string types.
            /// @param P String to assign.
            /// @return Reference to path
            Path &operator=(const std::filesystem::path &P);

            /// @brief Appends P to Path. Adds / if needed. Performs minor checks on P before appending.
            /// @param P String to append.
            /// @return Reference to path.
            Path &operator/=(const char *P);

            /// @brief Appends P to Path. Adds / if needed. Performs minor checks on P before appending.
            /// @param P String to append.
            /// @return Reference to path.
            Path &operator/=(const std::string &P);

            /// @brief Appends P to Path. Adds / if needed. Performs minor checks on P before appending.
            /// @param P String to append.
            /// @return Reference to path.
            Path &operator/=(std::string_view P);

            /// @brief Appends P to Path. Adds / if needed. Performs minor checks on P before appending.
            /// @param P String to append.
            /// @return Reference to path.
            Path &operator/=(const std::filesystem::path &P);

            /// @brief Appends P to Path without any checks or / added.
            /// @param P String to append.
            /// @return Reference to path.
            Path &operator+=(const char *P);

            /// @brief Appends P to Path without any checks or / added.
            /// @param P String to append.
            /// @return Reference to path.
            Path &operator+=(const std::string &P);

            /// @brief Appends P to Path without any checks or / added.
            /// @param P String to append.
            /// @return Reference to path.
            Path &operator+=(std::string_view P);

            /// @brief Appends P to Path without any checks or / added.
            /// @param P String to append.
            /// @return Reference to path.
            Path &operator+=(const std::filesystem::path &P);

            /**
             * @brief Value returned by Find[X]Of functions if the search fails.
             * @note This can be used two ways:
             *      1. FsLib::Path::NotFound
             *      2. [Path Instance].NotFound.
             */
            static constexpr uint16_t NotFound = -1;

        private:
            char *m_Path = nullptr;
            char *m_DeviceEnd = nullptr;
            // Neither of these are going to come close to touching 0xFFFF.
            uint16_t m_PathSize = 0;
            uint16_t m_PathLength = 0;
            // This allocates memory for the path.
            bool AllocatePath(uint16_t PathSize);
            // This frees the path data.
            void FreePath(void);
    };

    /// @brief Concatenates two paths. Adds a / if needed.
    /// @param Path1 Base path.
    /// @param Path2 Path to concatenate to Path1.
    /// @return New path consisting of both paths.
    FsLib::Path operator/(const FsLib::Path &Path1, const char *Path2);

    /// @brief Concatenates two paths. Adds a / if needed.
    /// @param Path1 Base path.
    /// @param Path2 Path to concatenate to Path1.
    /// @return New path consisting of both paths.
    FsLib::Path operator/(const FsLib::Path &Path1, const std::string &Path2);

    /// @brief Concatenates two paths. Adds a / if needed.
    /// @param Path1 Base path.
    /// @param Path2 Path to concatenate to Path1.
    /// @return New path consisting of both paths.
    FsLib::Path operator/(const FsLib::Path &Path1, std::string_view Path2);

    /// @brief Concatenates two paths. Adds a / if needed.
    /// @param Path1 Base path.
    /// @param Path2 Path to concatenate to Path1.
    /// @return New path consisting of both paths.
    FsLib::Path operator/(const FsLib::Path &Path1, const std::filesystem::path &Path2);

    /// @brief Unchecked concatenation operator. Doesn't perform checks or add / if needed.
    /// @param Path1 Base path.
    /// @param Path2 Path to concatenate to Path1
    /// @return New pat consisting of both paths.
    FsLib::Path operator+(const FsLib::Path &Path1, const char *Path2);

    /// @brief Unchecked concatenation operator. Doesn't perform checks or add / if needed.
    /// @param Path1 Base path.
    /// @param Path2 Path to concatenate to Path1
    /// @return New pat consisting of both paths.
    FsLib::Path operator+(const FsLib::Path &Path1, const std::string &Path2);

    /// @brief Unchecked concatenation operator. Doesn't perform checks or add / if needed.
    /// @param Path1 Base path.
    /// @param Path2 Path to concatenate to Path1
    /// @return New pat consisting of both paths.
    FsLib::Path operator+(const FsLib::Path &Path1, std::string_view Path2);

    /// @brief Unchecked concatenation operator. Doesn't perform checks or add / if needed.
    /// @param Path1 Base path.
    /// @param Path2 Path to concatenate to Path1
    /// @return New pat consisting of both paths.
    FsLib::Path operator+(const FsLib::Path &Path1, const std::filesystem::path &Path2);
} // namespace FsLib
