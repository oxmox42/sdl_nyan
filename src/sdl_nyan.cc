#include "sdl_nyan.h"

#include <SDL.h>
#include <SDL_render.h>
#include <array>
#include <cassert>
#include <cstdarg>
#include <string>
#include "log.h"
#include "nyan_types.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "nyan_data_r.c"

static const char *const NYAN_PATH = "../external/nyan/nyan";

static void nyan_sdl_fatal(const char *const msg)
{
    log_fatal("%s: %s", msg, SDL_GetError());
    abort();
}

static void nyan_sdl_error(const char *const msg)
{
    log_error("%s: %s", msg, SDL_GetError());
}

template<size_t Size>
[[maybe_unused]] const char *aprintf(std::array<char, Size> &buf, const char *fmt, ...)
{
    std::va_list args;
    va_start(args, fmt);
    std::vsnprintf(buf.data(), buf.size(), fmt, args);
    va_end(args);
    return buf.data();
}

static SDL_Texture *make_nyan_sprite_sheet_from_files(SDL_Renderer *renderer, const char dir = 'r')
{
    static const auto textureRect = nyan_sheet_rect();
    auto result = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                    textureRect.w, textureRect.h);

    if (!result)
        nyan_sdl_fatal("make_nyan_sprite_sheet_from_files/SDL_CreateTexture");

    if (SDL_SetTextureBlendMode(result, SDL_BLENDMODE_BLEND))
        nyan_sdl_fatal("make_nyan_sprite_sheet_from_files/SDL_SetTextureBlendMode");

    std::array<char, 1024> strBuf;

    for (size_t i=0; i<NYAN_SPRITE_COUNT; ++i)
    {
        auto filename = aprintf(strBuf, "%s/%c%zu.png", NYAN_PATH, dir, i+1);
        log_trace("make_nyan_sprite_sheet_from_files: loading %s", filename);
        int w = 0, h = 0, bytes_per_pixel = 0;
        u8 *data = stbi_load(filename, &w, &h, &bytes_per_pixel, 0);
        assert(w == NYAN_SPRITE_WIDTH && h == NYAN_SPRITE_HEIGHT && bytes_per_pixel == NYAN_BBP);
        auto destRect = nyan_sprite_rect(i);
        if (SDL_UpdateTexture(result, &destRect, data, NYAN_BBP * NYAN_SPRITE_WIDTH))
            nyan_sdl_fatal("make_nyan_sprite_sheet_from_files/SDL_UpdateTexture");
        STBI_FREE(data);
    }

    return result;
}

SDL_Texture *make_nyan_sprite_sheet_from_mem(SDL_Renderer *renderer)
{
    static const uint8_t *const data_ptrs[] = { r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, nullptr };

    static const size_t data_sizes[] = {
            sizeof(r1), sizeof(r2), sizeof(r3), sizeof(r4), sizeof(r5), sizeof(r6),
            sizeof(r7), sizeof(r8), sizeof(r9), sizeof(r10), sizeof(r11), sizeof(r12)
    };

    static const auto textureRect = nyan_sheet_rect();
    auto result = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                    textureRect.w, textureRect.h);

    if (!result)
        nyan_sdl_fatal("make_nyan_sprite_sheet_from_mem/SDL_CreateTexture");

    if (SDL_SetTextureBlendMode(result, SDL_BLENDMODE_BLEND))
        nyan_sdl_fatal("make_nyan_sprite_sheet_from_mem/SDL_SetTextureBlendMode");

    for (size_t i=0; data_ptrs[i]; ++i)
    {
        log_trace("make_nyan_sprite_sheet_from_mem: loading data %zu", i);
        int w = 0, h = 0, bytes_per_pixel = 0;
        u8 *data = stbi_load_from_memory(data_ptrs[i], data_sizes[i], &w, &h, &bytes_per_pixel, 0);
        assert(w == NYAN_SPRITE_WIDTH && h == NYAN_SPRITE_HEIGHT && bytes_per_pixel == NYAN_BBP);
        auto destRect = nyan_sprite_rect(i);
        if (SDL_UpdateTexture(result, &destRect, data, NYAN_BBP * NYAN_SPRITE_WIDTH))
            nyan_sdl_fatal("make_nyan_sprite_sheet_from_mem/SDL_UpdateTexture");
        STBI_FREE(data);
    }

    return result;
}
