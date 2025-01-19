#pragma once
#include "Color.hpp"
#include <SDL2/SDL.h>

namespace SDL
{
    namespace Text
    {
        // This is easier to read than just putting -1 in Render.
        static constexpr int NO_TEXT_WRAP = -1;
        // Inits FreeType and loads system font.
        bool Initialize(void);
        // Exits FreeType.
        void Exit(void);
        /*
            Renders text at X, Y with FontSize. Returns false if an error occurs while rendering text. GetErrorString can be checked for what happened.
            WrapWidth is the width text can take up before being wrapped to a new line. Passing NO_TEXT_WRAP will ignore this.
            TextColor is the color to use. The following characters will cause this function to change color in order to allow highlighting:
                # - Blue
                * - Red
                < - Yellow
                > - Green
            They should be followed by another of the same character after the text to highlight is finished to change back to the color passed.
        */
        void Render(SDL_Texture *Target, int X, int Y, int FontSize, int WrapWidth, SDL::Color TextColor, const char *Format, ...);
        // This returns the width of string passed.
        size_t GetWidth(int FontSize, const char *String);
        // Adds a special character and corrosponding color to change to when it's encountered.
        void AddColorCharacter(uint32_t Codepoint, SDL::Color Color);
    } // namespace Text
} // namespace SDL
