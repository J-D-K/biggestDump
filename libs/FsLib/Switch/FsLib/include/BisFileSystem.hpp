#pragma once
#include <string_view>
#include <switch.h>

namespace FsLib
{
    /// @brief Opens partition with ID passed.
    /// @param DeviceName Name to map to partition.
    /// @param PartitionID ID of BIS partition.
    /// @return True on success. False on failure.
    bool OpenBisFileSystem(std::string_view DeviceName, FsBisPartitionId PartitionID);
} // namespace FsLib
