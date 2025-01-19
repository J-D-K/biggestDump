#include "Text.hpp"
#include "SDL.hpp"
#include "String.hpp"
#include <algorithm>
#include <array>
#include <cstdarg>
#include <cstring>
#include <ft2build.h>
#include <map>
#include <string>
#include <switch.h>
#include FT_FREETYPE_H

namespace
{
    // FreeType library and faces.
    FT_Library s_FTLib;
    FT_Face s_FTFaces[PlSharedFontType_Total];
    // Number of fonts successfully loaded.
    int32_t s_TotalFonts = 0;
    // This struct holds the data needed to render each glyph.
    typedef struct
    {
            uint16_t Width, Height;
            int16_t AdvanceX, Top, Left;
            SDL::SharedTexture GlyphTexture;
    } GlyphData;
    // Glyph cache map. Mapped according to Codepoint and font size.
    std::map<std::pair<uint32_t, int>, GlyphData> s_FontCacheMap;
    // Map of characters that are assigned to change colors.
    std::unordered_map<uint32_t, SDL::Color> s_SpecialCharacterMap;
    // Buffer size to use with render.
    constexpr int VA_BUFFER_SIZE = 0x1000;
    constexpr int WORD_BUFFER_SIZE = 0x100;
    // Breakpoints for line breaking.
    constexpr std::array<uint32_t, 7> s_Breakpoints = {L' ', L'　', L'/', L'_', L'-', L'。', L'、'};
} // namespace

// Global error string
extern std::string g_SDLErrorString;

static inline bool FTErrorOccured(FT_Error Error)
{
    return Error != 0;
}

// Resizes FreeType faces to FontSize in pixels.
static bool ResizeFont(int FontSize)
{
    for (int32_t i = 0; i < s_TotalFonts; i++)
    {
        FT_Error FTError = FT_Set_Pixel_Sizes(s_FTFaces[i], 0, static_cast<FT_UInt>(FontSize));
        if (FTErrorOccured(FTError))
        {
            g_SDLErrorString = SDL::String::GetFormattedString("Error setting pixel size for font: %i.", FTError);
            return false;
        }
    }
    return true;
}

// Just returns if the codepoint passed is in the array of points to break a line at.
static inline bool IsBreakableCharacter(uint32_t Codepoint)
{
    if (std::find(s_Breakpoints.begin(), s_Breakpoints.end(), Codepoint) == s_Breakpoints.end())
    {
        return false;
    }
    return true;
}

// Finds the next available point to break at and returns a pointer to it.
static size_t FindNextBreakpoint(const char *String)
{
    uint32_t Codepoint = 0;
    size_t StringLength = std::strlen(String);
    for (size_t i = 0; i < StringLength;)
    {
        ssize_t UnitCount = decode_utf8(&Codepoint, reinterpret_cast<const uint8_t *>(&String[i]));
        i += UnitCount;

        if (UnitCount <= 0 || IsBreakableCharacter(Codepoint))
        {
            return i;
        }
    }
    return StringLength;
}

// Attempts to locate a glyph in the cache. If that fails, loads the glyph.
static GlyphData *LoadGetGlyph(uint32_t Codepoint, int FontSize)
{
    if (s_FontCacheMap.find(std::make_pair(Codepoint, FontSize)) != s_FontCacheMap.end())
    {
        return &s_FontCacheMap.at(std::make_pair(Codepoint, FontSize));
    }

    FT_GlyphSlot GlyphSlot = NULL;
    for (int32_t i = 0; i < s_TotalFonts; i++)
    {
        FT_UInt CodepointIndex = FT_Get_Char_Index(s_FTFaces[i], Codepoint);
        FT_Error FTError = FT_Load_Glyph(s_FTFaces[i], CodepointIndex, FT_LOAD_RENDER);
        if (CodepointIndex != 0 && !FTErrorOccured(FTError))
        {
            GlyphSlot = s_FTFaces[i]->glyph;
            break;
        }
    }

    // If it wasn't found, just NULL here.
    if (!GlyphSlot)
    {
        return NULL;
    }

    FT_Bitmap GlyphBitmap = GlyphSlot->bitmap;
    // This never happens, but JIC.
    if (GlyphBitmap.pixel_mode != FT_PIXEL_MODE_GRAY)
    {
        return NULL;
    }

    SDL_Surface *GlyphSurface =
        SDL_CreateRGBSurface(0, GlyphBitmap.width, GlyphBitmap.rows, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    if (!GlyphSurface)
    {
        return NULL;
    }

    // Pointers to Pixel data we're using here.
    size_t BitmapSize = GlyphBitmap.width * GlyphBitmap.rows;
    uint8_t *BitmapPointer = GlyphBitmap.buffer;
    uint32_t *SurfacePixels = reinterpret_cast<uint32_t *>(GlyphSurface->pixels);
    // Loop and fill Surface with white pixels using bitmap as alpha channel.
    for (size_t i = 0; i < BitmapSize; i++)
    {
        *SurfacePixels++ = 0xFFFFFF00 | *BitmapPointer++;
    }

    // Add it to cache map. Requires a string for manager.
    std::string GlyphName = std::to_string(Codepoint) + std::to_string(FontSize);
    s_FontCacheMap[std::make_pair(Codepoint, FontSize)] = {.Width = static_cast<uint16_t>(GlyphBitmap.width),
                                                           .Height = static_cast<uint16_t>(GlyphBitmap.rows),
                                                           .AdvanceX = static_cast<int16_t>(GlyphSlot->advance.x >> 6),
                                                           .Top = static_cast<int16_t>(GlyphSlot->bitmap_top),
                                                           .Left = static_cast<int16_t>(GlyphSlot->bitmap_left),
                                                           .GlyphTexture =
                                                               SDL::TextureManager::CreateLoadTexture(GlyphName, GlyphSurface, true)};

    return &s_FontCacheMap[std::make_pair(Codepoint, FontSize)];
}

// This function handles special characters that change text color.
static inline bool ProcessSpecialCharacters(uint32_t Codepoint, SDL::Color OriginalColor, SDL::Color &TextColor)
{
    if (s_SpecialCharacterMap.find(Codepoint) == s_SpecialCharacterMap.end())
    {
        return false;
    }

    if (TextColor.Raw == s_SpecialCharacterMap[Codepoint].Raw)
    {
        TextColor = OriginalColor;
    }
    else
    {
        TextColor = s_SpecialCharacterMap[Codepoint];
    }

    return true;
}

bool SDL::Text::Initialize(void)
{
    Result PlError = plInitialize(PlServiceType_User);
    if (R_FAILED(PlError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error initializing Pl: 0x%X.", PlError);
        return false;
    }

    // Init Freetype.
    FT_Error FTError = FT_Init_FreeType(&s_FTLib);
    if (FTErrorOccured(FTError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error initializing FreeType: %i.", FTError);
        return false;
    }

    // Get language code.
    Result SetError = setInitialize();
    if (R_FAILED(SetError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error initializing set for language code: 0x%X.", SetError);
        return false;
    }

    uint64_t LanguageCode = 0;
    SetError = setGetLanguageCode(&LanguageCode);
    if (R_FAILED(SetError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error getting system language code: 0x%X.", SetError);
        return false;
    }
    setExit();

    // Load shared fonts.
    PlFontData SharedFont[PlSharedFontType_Total];
    PlError = plGetSharedFont(LanguageCode, SharedFont, PlSharedFontType_Total, &s_TotalFonts);
    if (R_FAILED(PlError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error loading shared fonts: 0x%X.", PlError);
        return false;
    }

    // Loop through and init faces.
    for (int32_t i = 0; i < s_TotalFonts; i++)
    {
        FTError = FT_New_Memory_Face(s_FTLib, reinterpret_cast<const FT_Byte *>(SharedFont[i].address), SharedFont[i].size, 0, &s_FTFaces[i]);
        if (FTErrorOccured(FTError))
        {
            g_SDLErrorString = SDL::String::GetFormattedString("Error creating new memory face: %i.", FTError);
            return false;
        }
    }
    return true;
}

void SDL::Text::Exit(void)
{
    for (int32_t i = 0; i < s_TotalFonts; i++)
    {
        FT_Done_Face(s_FTFaces[i]);
    }
    FT_Done_FreeType(s_FTLib);
    plExit();
}

void SDL::Text::Render(SDL_Texture *Target, int X, int Y, int FontSize, int WrapWidth, SDL::Color TextColor, const char *Format, ...)
{
    int SDLError = SDL_SetRenderTarget(SDL::GetRenderer(), Target);
    if (SDL::ErrorOccured(SDLError))
    {
        g_SDLErrorString = SDL::String::GetFormattedString("Error setting render target: %s.", SDL_GetError());
        return;
    }

    // Need to preserve original coordinates and color.
    int WorkingX = X;
    int WorkingY = Y;
    SDL::Color WorkingColor = TextColor;

    ResizeFont(FontSize);

    std::array<char, VA_BUFFER_SIZE> VaBuffer = {0};
    std::va_list VaList;
    va_start(VaList, Format);
    vsnprintf(VaBuffer.data(), VA_BUFFER_SIZE, Format, VaList);
    va_end(VaList);

    size_t StringLength = std::strlen(VaBuffer.data());
    for (size_t i = 0; i < StringLength;)
    {
        // This is the buffer used for cutting the string up into words and wrapping if needed.
        std::array<char, WORD_BUFFER_SIZE> WordBuffer = {0};

        // Find the next break point in the string and then memcpy it to WordBuffer.
        size_t WordLength = FindNextBreakpoint(&VaBuffer.data()[i]);
        std::memcpy(WordBuffer.data(), &VaBuffer[i], WordLength);

        if (WrapWidth != SDL::Text::NO_TEXT_WRAP)
        {
            size_t WordWidth = SDL::Text::GetWidth(FontSize, WordBuffer.data());
            if (WorkingX + WordWidth >= static_cast<unsigned int>(X + WrapWidth))
            {
                WorkingX = X;
                WorkingY += FontSize + (FontSize / 3);
            }
        }

        uint32_t Codepoint = 0;
        for (size_t j = 0; j < WordLength;)
        {
            ssize_t UnitCount = decode_utf8(&Codepoint, reinterpret_cast<const uint8_t *>(&WordBuffer.data()[j]));
            if (UnitCount <= 0)
            {
                break;
            }

            i += UnitCount;
            j += UnitCount;
            if (ProcessSpecialCharacters(Codepoint, TextColor, WorkingColor))
            {
                continue;
            }
            else if (Codepoint == '\n')
            {
                WorkingX = X;
                WorkingY += FontSize + (FontSize / 3);
                continue;
            }

            GlyphData *Glyph = LoadGetGlyph(Codepoint, FontSize);
            // Space needs special handling because the texture is null.
            if (Glyph && Codepoint != L' ')
            {
                Glyph->GlyphTexture->SetColorMod(WorkingColor);
                Glyph->GlyphTexture->Render(Target, WorkingX + Glyph->Left, WorkingY + (FontSize - Glyph->Top));
                WorkingX += Glyph->AdvanceX;
            }
            else if (Glyph && Codepoint == L' ')
            {
                WorkingX += Glyph->AdvanceX;
            }
        }
    }
}

size_t SDL::Text::GetWidth(int FontSize, const char *String)
{
    uint32_t Codepoint = 0;
    size_t StringWidth = 0;
    size_t StringLength = std::strlen(String);
    // This is to pass to the process function.
    SDL::Color TempColor = {0x00000000};

    ResizeFont(FontSize);

    for (size_t i = 0; i < StringLength;)
    {
        ssize_t UnitCount = decode_utf8(&Codepoint, reinterpret_cast<const uint8_t *>(&String[i]));
        if (UnitCount <= 0)
        {
            return StringWidth;
        }

        i += UnitCount;
        if (ProcessSpecialCharacters(Codepoint, TempColor, TempColor) || Codepoint == L'\n')
        {
            continue;
        }

        GlyphData *Glyph = LoadGetGlyph(Codepoint, FontSize);
        if (Glyph)
        {
            StringWidth += Glyph->AdvanceX;
        }
    }
    return StringWidth;
}

void SDL::Text::AddColorCharacter(uint32_t Codepoint, SDL::Color Color)
{
    s_SpecialCharacterMap[Codepoint] = Color;
}
