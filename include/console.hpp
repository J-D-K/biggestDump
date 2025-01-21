#pragma once
#include "sdl.hpp"
#include <cstdarg>
#include <cstddef>
#include <cstring>
#include <string>

class Console
{
    public:
        // No copying.
        Console(const Console &) = delete;
        Console(Console &&) = delete;
        Console &operator=(const Console &) = delete;
        Console &operator=(Console &&) = delete;

        // Sets the X and Y coordinates the console is rendered at.
        static void setXY(int x, int y)
        {
            Console &console = Console::getInstance();
            console.m_x = x;
            console.m_y = y;
        }

        // Sets the maximum amount of lines that should be displayed before we start clipping.
        static void setMaxLineCount(size_t maxLineCount)
        {
            Console &console = Console::getInstance();
            console.m_maxLineCount = maxLineCount;
        }

        // Sets the size of the font in pixels. Default is 20 pixels.
        static void setFontSize(int fontSize)
        {
            Console &console = Console::getInstance();
            console.m_fontSize = fontSize;
        }

        // Sets the size of the buffer used for printf. Default is 0x1000 and that should be plenty.
        static void setBufferSize(size_t bufferSize)
        {
            Console &console = Console::getInstance();
            console.m_bufferSize = bufferSize;
        }

        // Sets the color used to render text.
        static void setRenderColor(sdl::Color color)
        {
            Console &console = Console::getInstance();
            console.m_renderColor = color;
        }

        static void printf(const char *format, ...)
        {
            Console &console = Console::getInstance();

            // Va
            char vaBuffer[console.m_bufferSize] = {0};
            std::va_list vaList;
            va_start(vaList, format);
            vsnprintf(vaBuffer, console.m_bufferSize, format, vaList);
            va_end(vaList);

            // Get count of how many lines are in the string.
            size_t newLineCount = 0;
            const char *newLineSearch = vaBuffer;
            while ((newLineSearch = std::strchr(newLineSearch, '\n')) != NULL)
            {
                ++newLineCount;
                ++newLineSearch;
            }

            // Add new lines to count.
            console.m_lineCount += newLineCount;

            if (console.m_lineCount >= console.m_maxLineCount)
            {
                // Number of lines to trim from string.
                size_t trimLines = console.m_lineCount - console.m_maxLineCount;

                for (size_t i = 0, currentLinePosition = 0; i < trimLines; i++)
                {
                    // Find position of next line to trim off the top.
                    currentLinePosition = console.m_consoleString.find_first_of('\n', currentLinePosition) + 1;

                    // Trim it or break the loop if something weird happens.
                    if (currentLinePosition == console.m_consoleString.npos)
                    {
                        break;
                    }
                    // Trim the line from the beginning.
                    console.m_consoleString.erase(0, currentLinePosition);
                    // Go forward one character in the buffer/string and subtract the lines we trimmed.
                    ++currentLinePosition;
                    console.m_lineCount -= trimLines;
                }
            }
            // Append our va string from earlier.
            console.m_consoleString += vaBuffer;
        }

        // Renders the console directly to the main framebuffer target.
        static void render(void)
        {
            Console &console = Console::getInstance();
            sdl::text::render(NULL,
                              console.m_x,
                              console.m_y,
                              console.m_fontSize,
                              sdl::text::NO_TEXT_WRAP,
                              console.m_renderColor,
                              console.m_consoleString.c_str());
        }

        // Resets and clears the console.
        static void reset(void)
        {
            Console &console = Console::getInstance();
            console.m_lineCount = 0;
            console.m_consoleString.clear();
        }

    private:
        // No constructing.
        Console(void) = default;
        // Returns instance.
        static Console &getInstance(void)
        {
            static Console console;
            return console;
        }
        // X and Y coordinates.
        int m_x = 0, m_y = 0;
        // Line counts
        size_t m_lineCount = 0;
        size_t m_maxLineCount = 0;
        // Font size
        int m_fontSize = 20;
        // Size of the vaBuffer.
        size_t m_bufferSize = 0x1000;
        // Color used to render. Default is white.
        sdl::Color m_renderColor = {0xFFFFFFFF};
        // Console string
        std::string m_consoleString;
};
