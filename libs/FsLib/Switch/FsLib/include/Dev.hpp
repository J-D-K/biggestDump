#pragma once

namespace FsLib
{
    /// @brief Contains the function for overriding fs_dev.
    namespace Dev
    {
        /**
         * @brief Initializes a bare-bones compatibility layer so devkitPro libs can read and write files on the SD card.
         *
         * @return True on success. False on failure.
         * @note This doesn't work like on 3DS. Overriding __appInit on Switch seems to cause __libnx_init to not get called too. I'm assuming
         * this is for sysmodules. Instead, it shuts down fs_dev and installs its own sdmc device in newlib. I chose this way, because the
         * end result is cleaner for the end user. Having to implement a complete version of LibNX's initialization code just to bypass one
         * function was a nightmare. Don't ask me how I know...
         */
        bool InitializeSDMC(void);
    } // namespace Dev
} // namespace FsLib
