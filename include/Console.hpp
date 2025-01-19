#pragma once
#include "SDL.hpp"
#include <cstdarg>
#include <cstddef>
#include <cstring>
#include <string>

/*
    biggestDump's console but modified.
*/

class Console
{
    public:
        // No copying.
        Console(const Console &) = delete;
        Console(Console &&) = delete;
        Console &operator=(const Console &) = delete;
        Console &operator=(Console &&) = delete;

        static void SetMaxLineCount(size_t MaxLineCount)
        {
            Console &Instance = Console::GetInstance();

            Instance.m_MaxLineCount = MaxLineCount;
        }

        static void SetFontSize(int FontSize)
        {
            Console &Instance = Console::GetInstance();

            Instance.m_FontSize = FontSize;
        }

        // I considered rendering tricks to accomplish this, but this is easier.
        static void Printf(const char *Format, ...)
        {
            Console &Instance = Console::GetInstance();

            char VaBuffer[0x1000];
            std::va_list VaList;
            va_start(VaList, Format);
            vsnprintf(VaBuffer, 0x1000, Format, VaList);
            va_end(VaList);

            // This is to get count of how many lines to remove.
            size_t NewLineCount = 0;
            char *NewLineSearch = VaBuffer;
            while ((NewLineSearch = std::strchr(NewLineSearch, '\n')) != NULL)
            {
                ++NewLineCount;
                ++NewLineSearch;
            }

            Instance.m_LineCount += NewLineCount;

            // This is to actually delete that many lines from the Console's string.
            if (Instance.m_LineCount >= Instance.m_MaxLineCount)
            {
                // Number of lines to trim
                size_t LinesToTrim = Instance.m_LineCount - Instance.m_MaxLineCount;

                for (size_t i = 0, CurrentLinePosition = 0; i < LinesToTrim; i++)
                {
                    CurrentLinePosition = Instance.m_ConsoleText.find_first_of('\n', CurrentLinePosition) + 1;
                    if (CurrentLinePosition != Instance.m_ConsoleText.npos)
                    {
                        Instance.m_ConsoleText.erase(0, CurrentLinePosition);
                    }
                    else
                    {
                        break;
                    }
                    ++CurrentLinePosition;
                    Instance.m_LineCount -= LinesToTrim;
                }
            }
            // Append buffer to console output.
            Instance.m_ConsoleText += VaBuffer;
        }

        // This is the render function. Normally this would be public, but in this case, we don't want it called outside of the class.
        static void Render(void)
        {
            Console &Instance = Console::GetInstance();
            SDL::Text::Render(NULL, 56, 94, Instance.m_FontSize, SDL::Text::NO_TEXT_WRAP, {0xFFFFFFFF}, Instance.m_ConsoleText.c_str());
        }

        static void Reset(void)
        {
            Console &Instance = Console::GetInstance();

            Instance.m_LineCount = 0;
            Instance.m_ConsoleText.clear();
        }

    private:
        // No constructing.
        Console(void) = default;
        // Returns the only instance of console.
        static Console &GetInstance(void)
        {
            static Console Instance;
            return Instance;
        }
        // Number of lines before we stand cutting them off.
        size_t m_LineCount = 0;
        size_t m_MaxLineCount = 0;
        // Font size used to render text. Default is 20, but this can be changed by calling SetFontSize.
        int m_FontSize = 20;
        // Actual text being displayed.
        std::string m_ConsoleText;
};
