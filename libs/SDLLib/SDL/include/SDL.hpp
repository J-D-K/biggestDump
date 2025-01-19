#pragma once
#include "Color.hpp"
#include "Text.hpp"
#include "TextureManager.hpp"
#include <SDL2/SDL.h>

namespace SDL
{
    // Initializes SDL and SDL_image.
    bool Initialize(const char *WindowTitle, int WindowWidth, int WindowHeight);
    // Exits SDL
    void Exit(void);
    // Returns internal error string.
    const char *GetErrorString(void);
    // Returns renderer if needed.
    SDL_Renderer *GetRenderer(void);
    // Begins and frame and clears the renderer to ClearColor
    bool FrameBegin(SDL::Color ClearColor);
    // Ends and frame and presents it to screen.
    void FrameEnd(void);
    // I don't feel like these need a separate file.
    bool RenderLine(SDL_Texture *Target, int X1, int Y1, int X2, int Y2, SDL::Color LineColor);
    bool RenderRectFill(SDL_Texture *Target, int X, int Y, int Width, int Height, SDL::Color RectColor);

    // I got tired of type SDLError != 0.
    inline bool ErrorOccured(int SDLError)
    {
        return SDLError != 0;
    }
} // namespace SDL
