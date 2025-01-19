#pragma once
#include "Color.hpp"
#include <SDL2/SDL.h>

namespace SDL
{
    class Texture
    {
        public:
            // The texture manager class uses a template function and these overloads to create and return textures.
            // Loads texture from path passed.
            Texture(const char *ImagePath);
            // Creates a texture from the Surface passed. Free is whether or not this function should free the surface afterwards.
            Texture(SDL_Surface *Surface, bool Free);
            // Creates a texture from the Data passed.
            Texture(const void *Data, size_t DataSize);
            // Creates a blank texture Width and Height in size with AccessFlags.
            Texture(int Width, int Height, int SDLAccessFlags);
            // Frees the texture.
            ~Texture();

            // Returns the underlying SDL_Texture pointer.
            SDL_Texture *Get(void);

            // Renders texture as-is to X and Y.
            bool Render(SDL_Texture *Target, int X, int Y);
            // Renders texture stretched to Width and Height at X and Y.
            bool RenderStretched(SDL_Texture *Target, int X, int Y, int Width, int Height);
            // Renders a part of the texture at X and Y.
            bool RenderPart(SDL_Texture *Target, int X, int Y, int SourceX, int SourceY, int SourceWidth, int SourceHeight);
            // Renders part of the texture stretched at X and Y with width and height.
            bool RenderPartStretched(SDL_Texture *Target,
                                     int SourceX,
                                     int SourceY,
                                     int SourceWidth,
                                     int SourceHeight,
                                     int DestinationX,
                                     int DestinationY,
                                     int DestinationWidth,
                                     int DestinationHeight);

            // Clears texture to ClearColor
            bool Clear(SDL::Color ClearColor);
            // Resizes the texture to Width and Height with SDLAccessFlags
            bool Resize(int Width, int Height, int SDLAccessFlags);
            // Sets color mod to texture.
            bool SetColorMod(SDL::Color ColorMod);

        private:
            // Underlying texture.
            SDL_Texture *m_Texture;
            // Width and height so I don't need to query for it.
            int16_t m_Width, m_Height;
    };
} // namespace SDL
