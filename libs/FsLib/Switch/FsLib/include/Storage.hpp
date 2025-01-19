#pragma once
#include "Stream.hpp"
#include <switch.h>

namespace FsLib
{
    /**
     * @brief Class for reading from raw storage volumes.
     * @note This still needs a lot of work for the different storage types. This class is <b>read only</b> for a reason.
     * Even with that, it can still <b>read the most sensitive data</b> on the Switch.
    */
    class Storage : public FsLib::Stream
    {
        public:
            /// @brief  Default storage constructor.
            Storage(void) = default;

            /// @brief Attempts to open PartitionID as raw storage.
            /// @param PartitionID ID of partition to open.
            /// @note IsOpen can be used to see if the operation succeeded.
            Storage(FsBisPartitionId PartitionID);

            /// @brief Closes storage handle at destruction.
            ~Storage();

            /// @brief Attempts to open PartitionID as raw storage.
            /// @param PartitionID ID of partition to open.
            /// @note IsOpen can be used to see if the operation succeeded.
            void Open(FsBisPartitionId PartitionID);

            /// @brief Closes storage handle.
            void Close(void);

            /**
             * @brief Attempts to read from storage.
             *
             * @param Buffer Buffer to read into.
             * @param BufferSize Size of Buffer.
             * @return Number of bytes read.
             * @note The underlying Switch storage reading functions have no way to really keep track of how much was read or where you
             * are located (offset). I've done the best I can to correct for this.
             */
            size_t Read(void *Buffer, size_t BufferSize);

            /// @brief Reads a single byte from storage.
            /// @return Byte read on success. -1 on failure.
            signed char ReadByte(void);

        private:
            /// @brief Handle to storage opened.
            FsStorage m_StorageHandle;
    };
} // namespace FsLib
