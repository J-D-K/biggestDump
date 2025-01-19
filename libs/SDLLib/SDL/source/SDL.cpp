#include "SDL.hpp"
#include "String.hpp"
#include <SDL2/SDL_image.h>
#include <string>

namespace
{
    // Window and renderer.
    SDL_Window *s_Window = NULL;
    SDL_Renderer *s_Renderer = NULL;
    // These are the flags for SDL_image
    constexpr int SDL_IMAGE_FLAGS = IMG_INIT_JPG | IMG_INIT_PNG;
    // These are error strings I don't want to type over and over.
    const char *ERROR_SETTING_RENDER_TARGET = "Error setting renderer target: %s";
    const char *ERROR_SETTING_RENDER_COLOR = "Error setting renderer draw color: %s.";
} // namespace

std::string g_SDLErrorString = "No errors.";

bool SDL::Initialize(const char *WindowTitle, int WindowWidth, int WindowHeight)
{
    int SDLError = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error initializing SDL: %s.", SDL_GetError());
        return false;
    }

    // I do this for more control over renderer flags.
    s_Window = SDL_CreateWindow(WindowTitle, 0, 0, WindowWidth, WindowHeight, 0);
    if (!s_Window)
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error creating SDL_Window: %s.", SDL_GetError());
        return false;
    }

    s_Renderer = SDL_CreateRenderer(s_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!s_Renderer)
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error creating SDL_Renderer: %s.", SDL_GetError());
        return false;
    }

    SDLError = IMG_Init(SDL_IMAGE_FLAGS);
    if (SDLError != SDL_IMAGE_FLAGS)
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error initializing SDL_image: %s.", IMG_GetError());
        return false;
    }

    SDL_bool HintError = SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
    if (!HintError)
    {
        // I don't know if this can fail or if it writes the string...
        g_SDLErrorString = SDL::String::GetFormattedString("Error setting texture scale quality: %s.", SDL_GetError());
        // Not gonna return failure for this. Even if it does look ugly.
    }

    SDLError = SDL_SetRenderDrawBlendMode(s_Renderer, SDL_BLENDMODE_BLEND);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error setting SDL blend mode: %s.", SDL_GetError());
    }

    return true;
}

void SDL::Exit(void)
{
    if (s_Renderer)
    {
        SDL_DestroyRenderer(s_Renderer);
    }

    if (s_Window)
    {
        SDL_DestroyWindow(s_Window);
    }
    IMG_Quit();
    SDL_Quit();
}

const char *SDL::GetErrorString(void)
{
    return g_SDLErrorString.c_str();
}

SDL_Renderer *SDL::GetRenderer(void)
{
    return s_Renderer;
}

bool SDL::FrameBegin(SDL::Color ClearColor)
{
    int SDLError = SDL_SetRenderTarget(s_Renderer, NULL);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_SETTING_RENDER_TARGET, SDL_GetError());
        return false;
    }

    SDLError = SDL_SetRenderDrawColor(s_Renderer, ClearColor.RGBA[3], ClearColor.RGBA[2], ClearColor.RGBA[1], ClearColor.RGBA[0]);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_SETTING_RENDER_COLOR, SDL_GetError());
        return false;
    }

    SDLError = SDL_RenderClear(s_Renderer);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error clearing renderer: %s.", SDL_GetError());
        return false;
    }

    return true;
}

void SDL::FrameEnd(void)
{
    SDL_RenderPresent(s_Renderer);
}

bool SDL::RenderLine(SDL_Texture *Target, int X1, int Y1, int X2, int Y2, SDL::Color LineColor)
{
    int SDLError = SDL_SetRenderTarget(s_Renderer, Target);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_SETTING_RENDER_TARGET, SDL_GetError());
        return false;
    }

    SDLError = SDL_SetRenderDrawColor(s_Renderer, LineColor.RGBA[3], LineColor.RGBA[2], LineColor.RGBA[1], LineColor.RGBA[0]);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_SETTING_RENDER_COLOR, SDL_GetError());
        return false;
    }

    SDLError = SDL_RenderDrawLine(s_Renderer, X1, Y1, X2, Y2);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error rendering line: %s.", SDL_GetError());
        return false;
    }
    return true;
}

bool SDL::RenderRectFill(SDL_Texture *Target, int X, int Y, int Width, int Height, SDL::Color RectColor)
{
    int SDLError = SDL_SetRenderTarget(s_Renderer, Target);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_SETTING_RENDER_TARGET, SDL_GetError());
        return false;
    }

    SDLError = SDL_SetRenderDrawColor(s_Renderer, RectColor.RGBA[3], RectColor.RGBA[2], RectColor.RGBA[1], RectColor.RGBA[0]);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_SETTING_RENDER_COLOR, SDL_GetError());
        return false;
    }

    SDL_Rect RectCoordinates = {.x = X, .y = Y, .w = Width, .h = Height};
    SDLError = SDL_RenderFillRect(s_Renderer, &RectCoordinates);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error rendering rectangle: %s.", SDL_GetError());
        return false;
    }
    return true;
}
