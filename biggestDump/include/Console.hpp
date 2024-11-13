#pragma once
#include "SDL.hpp"
#include <cstddef>
#include <cstring>
#include <mutex>
#include <string>

class Console
{
    public:
        // No copying.
        Console(const Console &) = delete;
        Console(Console &&) = delete;
        Console &operator=(const Console &) = delete;
        Console &operator=(Console &&) = delete;

        // Unfortunately, this isn't going to work how I originally wanted. This is the closest I can do...
        static Console &Get(void)
        {
            return Console::GetInstance();
        }

        static void SetMaxLineCount(size_t MaxLineCount)
        {
            Console &Instance = Console::GetInstance();

            std::lock_guard<std::mutex> ConsoleLock(Instance.m_ConsoleMutex);
            Instance.m_MaxLineCount = MaxLineCount;
        }

        static void Render(void)
        {
            Console &Instance = Console::GetInstance();

            //std::lock_guard<std::mutex> ConsoleLock(Instance.m_ConsoleMutex);
            SDL::Text::Render(NULL, 56, 94, 20, 1068, {0xFFFFFFFF}, Instance.m_ConsoleText.c_str());
        }

        // Adds and prints text to the console. Always use << "\n" or << Console::NewLine at the end of a line so this works properly.
        Console &operator<<(const char *String)
        {
            std::lock_guard<std::mutex> ConsoleLock(m_ConsoleMutex);

            // To do: A better way of handling new lines.
            size_t FirstLineEnd = m_ConsoleText.find_first_of('\n');
            if (std::strcmp(String, "\n") == 0 && ++m_LineCount >= m_MaxLineCount && FirstLineEnd != m_ConsoleText.npos)
            {
                m_ConsoleText.erase(0, FirstLineEnd + 1);
                --m_LineCount;
            }
            m_ConsoleText += String;
            return *this;
        }

        Console &operator<<(const std::string &String)
        {
            return *this << String.c_str();
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
        // Actual text being displayed.
        std::string m_ConsoleText;
        // Mutex because biggestDump is threaded.
        std::mutex m_ConsoleMutex;
};
