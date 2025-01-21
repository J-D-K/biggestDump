#pragma once

namespace thread
{
    // Dumps firmware to the sd card in a folder. The bool passed is so the function can signal it's finished.
    void dumpToFolder(bool *isRunning);
    // Dumps firmware, but writes it to a zip uncompressed. Bool is same as above.
    void dumpToZip(bool *isRunning);
} // namespace thread
