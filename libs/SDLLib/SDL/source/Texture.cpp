#include "Texture.hpp"
#include "SDL.hpp"
#include "String.hpp"
#include <SDL2/SDL_image.h>

namespace
{
    const char *ERROR_SETTING_RENDER_TARGET = "Error setting renderer target: %s";
    const char *ERROR_CREATING_TEXTURE_FROM_SURFACE = "Error creating texture from surface: %s.";
    const char *ERROR_QUERYING_TEXTURE = "Error querying texture: %s";
    const char *ERROR_RENDERING_TEXTURE = "Error rendering texture: %s";
} // namespace

// Global error string
extern std::string g_SDLErrorString;

SDL::Texture::Texture(const char *ImagePath)
{
    m_Texture = IMG_LoadTexture(SDL::GetRenderer(), ImagePath);
    if (!m_Texture)
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error loading texture from file: %s.", IMG_GetError());
    }

    int SDLError = SDL_QueryTexture(m_Texture, NULL, NULL, reinterpret_cast<int *>(&m_Width), reinterpret_cast<int *>(&m_Height));
    if (SDLError != 0)
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_QUERYING_TEXTURE, SDL_GetError());
    }
}

SDL::Texture::Texture(SDL_Surface *Surface, bool Free)
{
    m_Width = Surface->w;
    m_Height = Surface->h;

    m_Texture = SDL_CreateTextureFromSurface(SDL::GetRenderer(), Surface);
    if (!m_Texture)
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_CREATING_TEXTURE_FROM_SURFACE, SDL_GetError());
    }

    if (Free)
    {
        SDL_FreeSurface(Surface);
    }
}

SDL::Texture::Texture(const void *Data, size_t DataSize)
{
    SDL_RWops *ImageReadOps = SDL_RWFromConstMem(Data, DataSize);
    // I'm assuming this reads the data header and determines what the format is?
    m_Texture = IMG_LoadTexture_RW(SDL::GetRenderer(), ImageReadOps, 1);

    if (!m_Texture)
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error loading texture from memory: %s.", IMG_GetError());
    }

    int SDLError = SDL_QueryTexture(m_Texture, NULL, NULL, reinterpret_cast<int *>(&m_Width), reinterpret_cast<int *>(&m_Height));
    if (SDLError != 0)
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_QUERYING_TEXTURE, SDL_GetError());
    }
}

SDL::Texture::Texture(int Width, int Height, int SDLAccessFlags)
{
    m_Width = Width;
    m_Height = Height;

    m_Texture = SDL_CreateTexture(SDL::GetRenderer(), SDL_PIXELFORMAT_RGBA8888, SDLAccessFlags, Width, Height);
    if (!m_Texture)
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error creating texture: %s.", SDL_GetError());
    }
}

SDL::Texture::~Texture()
{
    // I'm pretty sure SDL checks for NULL with this...
    SDL_DestroyTexture(m_Texture);
}

SDL_Texture *SDL::Texture::Get(void)
{
    return m_Texture;
}

bool SDL::Texture::Render(SDL_Texture *Target, int X, int Y)
{
    int SDLError = SDL_SetRenderTarget(SDL::GetRenderer(), Target);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_SETTING_RENDER_TARGET, SDL_GetError());
        return false;
    }

    SDL_Rect SourceRect = {.x = 0, .y = 0, .w = m_Width, .h = m_Height};
    SDL_Rect DestinationRect = {.x = X, .y = Y, .w = m_Width, .h = m_Height};
    SDLError = SDL_RenderCopy(SDL::GetRenderer(), m_Texture, &SourceRect, &DestinationRect);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_RENDERING_TEXTURE, SDL_GetError());
        return false;
    }
    return true;
}

bool SDL::Texture::RenderStretched(SDL_Texture *Target, int X, int Y, int Width, int Height)
{
    int SDLError = SDL_SetRenderTarget(SDL::GetRenderer(), Target);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_SETTING_RENDER_TARGET, SDL_GetError());
        return false;
    }

    SDL_Rect SourceRect = {.x = 0, .y = 0, .w = m_Width, .h = m_Height};
    SDL_Rect DestinationRect = {.x = X, .y = Y, .w = Width, .h = Height};
    SDLError = SDL_RenderCopy(SDL::GetRenderer(), Target, &SourceRect, &DestinationRect);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_RENDERING_TEXTURE, SDL_GetError());
        return false;
    }
    return true;
}

bool SDL::Texture::RenderPart(SDL_Texture *Target, int X, int Y, int SourceX, int SourceY, int SourceWidth, int SourceHeight)
{
    int SDLError = SDL_SetRenderTarget(SDL::GetRenderer(), Target);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_SETTING_RENDER_TARGET, SDL_GetError());
        return false;
    }

    SDL_Rect SourceRect = {.x = SourceX, .y = SourceY, .w = SourceWidth, .h = SourceHeight};
    SDL_Rect DestinationRect = {.x = X, .y = Y, .w = SourceWidth, .h = SourceHeight};
    SDLError = SDL_RenderCopy(SDL::GetRenderer(), m_Texture, &SourceRect, &DestinationRect);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_RENDERING_TEXTURE, SDL_GetError());
        return false;
    }
    return true;
}

bool SDL::Texture::RenderPartStretched(SDL_Texture *Target,
                                       int SourceX,
                                       int SourceY,
                                       int SourceWidth,
                                       int SourceHeight,
                                       int DestinationX,
                                       int DestinationY,
                                       int DestinationWidth,
                                       int DestinationHeight)
{
    int SDLError = SDL_SetRenderTarget(SDL::GetRenderer(), Target);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_SETTING_RENDER_TARGET, SDL_GetError());
        return false;
    }

    SDL_Rect SourceRect = {.x = SourceX, .y = SourceY, .w = SourceWidth, .h = SourceHeight};
    SDL_Rect DestinationRect = {.x = DestinationX, .y = DestinationY, .w = DestinationWidth, .h = DestinationHeight};
    SDLError = SDL_RenderCopy(SDL::GetRenderer(), m_Texture, &SourceRect, &DestinationRect);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_RENDERING_TEXTURE, SDL_GetError());
        return false;
    }
    return true;
}

// This was a needed last second thing. Probably works fine, but testing later.
bool SDL::Texture::Clear(SDL::Color ClearColor)
{
    int SDLError = SDL_SetRenderDrawColor(SDL::GetRenderer(), ClearColor.RGBA[3], ClearColor.RGBA[2], ClearColor.RGBA[1], ClearColor.RGBA[0]);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error setting renderer draw color: %s.", SDL_GetError());
        return false;
    }

    SDLError = SDL_SetRenderTarget(SDL::GetRenderer(), m_Texture);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString(ERROR_SETTING_RENDER_TARGET, SDL_GetError());
        return false;
    }

    SDLError = SDL_RenderClear(SDL::GetRenderer());
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error clearing renderer: %s.", SDL_GetError());
        return false;
    }
    return true;
}

bool SDL::Texture::Resize(int Width, int Height, int SDLAccessFlags)
{
    // Destroy the current texture.
    SDL_DestroyTexture(m_Texture);

    // Create a new one.
    m_Texture = SDL_CreateTexture(SDL::GetRenderer(), SDL_PIXELFORMAT_RGBA8888, SDLAccessFlags, Width, Height);
    if (!m_Texture)
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error creating texture: %s.", SDL_GetError());
        return false;
    }
    return true;
}

bool SDL::Texture::SetColorMod(SDL::Color ColorMod)
{
    int SDLError = SDL_SetTextureColorMod(m_Texture, ColorMod.RGBA[3], ColorMod.RGBA[2], ColorMod.RGBA[1]);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error setting color mod: %s.", SDL_GetError());
        return false;
    }
    return true;
}
