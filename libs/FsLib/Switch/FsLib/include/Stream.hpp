#pragma once
#include <cstdint>

namespace FsLib
{
    /// @brief This is the base class all File and storage types are derived from.
    class Stream
    {
        public:
            /// @brief Default Stream constructor.
            Stream(void) = default;

            /// @brief Checks if stream was successfully opened.
            /// @return True on success. False on failure.
            bool IsOpen(void) const;

            /// @brief Gets the current offset in the stream.
            /// @return Current offset of the stream.
            int64_t Tell(void) const;

            /// @brief Gets the size of the current stream.
            /// @return Stream's size.
            int64_t GetSize(void) const;

            /// @brief Returns if the end of the stream has been reached.
            /// @return True if end of stream has been reached. False if it hasn't.
            bool EndOfStream(void) const;

            /**
             * @brief Seeks to Offset relative to Origin
             *
             * @param Offset Offset to seek to.
             * @param Origin Origin from whence to seek.
             * @note Origin can be one of the following:
             *      1. FsLib::SeekOrigin::Beginning
             *      2. FsLib::SeekOrigin::Current
             *      3. FsLib::SeekOrigin::End
             */
            void Seek(int64_t Offset, uint8_t Origin);

            /// @brief Used to seek from the beginning of the stream.
            static constexpr uint8_t Beginning = 0;
            /// @brief Used to seek from the current offset of the stream.
            static constexpr uint8_t Current = 1;
            /// @brief Used to seek from the end of the stream.
            static constexpr uint8_t End = 2;

        protected:
            /// @brief Current offset in stream.
            int64_t m_Offset = 0;

            /// @brief Total size of the stream being accessed.
            int64_t m_StreamSize = 0;

            /// @brief Whether or not opening the stream was successful.
            /// @note This is handled by derived classes.
            bool m_IsOpen = false;

            /// @brief Ensures offset isn't out of bounds after a seek is performed.
            void EnsureOffsetIsValid(void);
    };
} // namespace FsLib
