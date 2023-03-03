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

static const char *const NYAN_PATH = "../external/nyan/nyan";
static const size_t NYAN_SPRITE_COUNT = 12;
static const size_t NYAN_SPRITE_WIDTH = 36;
static const size_t NYAN_SPRITE_HEIGHT = 26;
static const size_t NYAN_BBP = 4;

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

static constexpr SDL_Rect nyan_sprite_rect(size_t index)
{
    return SDL_Rect{ static_cast<int>(NYAN_SPRITE_WIDTH * index), 0, NYAN_SPRITE_WIDTH, NYAN_SPRITE_HEIGHT};
}

static constexpr SDL_Rect nyan_sheet_rect()
{
    return { 0, 0, NYAN_SPRITE_COUNT * NYAN_SPRITE_WIDTH, NYAN_SPRITE_HEIGHT };
}

static SDL_Texture *make_nyan_texture(SDL_Renderer *renderer, const char dir = 'r')
{
    static const auto textureRect = nyan_sheet_rect();
    auto result = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                    textureRect.w, textureRect.h);

    if (!result)
        nyan_sdl_fatal("make_nyan_texture/SDL_CreateTexture");

    if (SDL_SetTextureBlendMode(result, SDL_BLENDMODE_BLEND))
        nyan_sdl_fatal("make_nyan_texture/SDL_SetTextureBlendMode");

    std::array<char, 1024> strBuf;

    for (size_t i=0; i<NYAN_SPRITE_COUNT; ++i)
    {
        auto filename = aprintf(strBuf, "%s/%c%zu.png", NYAN_PATH, dir, i+1);
        log_trace("make_nyan_texture: loading %s", filename);
        int w = 0, h = 0, bytes_per_pixel = 0;
        u8 *data = stbi_load(filename, &w, &h, &bytes_per_pixel, 0);
        assert(w == NYAN_SPRITE_WIDTH && h == NYAN_SPRITE_HEIGHT && bytes_per_pixel == NYAN_BBP);
        auto destRect = nyan_sprite_rect(i);
        if (SDL_UpdateTexture(result, &destRect, data, NYAN_BBP * NYAN_SPRITE_WIDTH))
            nyan_sdl_fatal("make_nyan_texture/SDL_UpdateTexture");
    }

    return result;
}

static const float NYAN_PI = 3.14159265358979323846f;
static const float NYAN_PI2 = 2.0f * NYAN_PI;

template<typename T> T deg2rad(T deg) { return deg * NYAN_PI / 180.0; }
template<typename T> T rad2deg(T deg) { return 180.0 * deg / NYAN_PI; }

struct NyanSpinnyCircle
{
    SDL_Texture *nyanSheet = nullptr;
    SDL_Point centerPos;
    float radius = 100.0f;
    float radiusBounce = 0.0f;
    float radiusBounceIncrement = 0.4f;
    float radiusBounceMax = 42.0f;
    float angle = 0.0f;
    float angularStep = 0.015f;
    unsigned animSpeed = 48;
    unsigned nyanCount = 13;
};

void do_circle_nyan_step(SDL_Renderer *renderer, NyanSpinnyCircle &nsc)
{
    static constexpr SDL_Point rotCenter = {NYAN_SPRITE_WIDTH, NYAN_SPRITE_HEIGHT};
    const size_t nyanSpriteIndex = (SDL_GetTicks() / nsc.animSpeed) % NYAN_SPRITE_COUNT;
    const auto sourceRect = nyan_sprite_rect(nyanSpriteIndex);
    const auto nyanRads = deg2rad(360.0f / nsc.nyanCount);

    for (auto i=0u; i<nsc.nyanCount; ++i)
    {
        auto a = nsc.angle + nyanRads * i;
        auto x = nsc.centerPos.x + std::cos(a) * (nsc.radius + nsc.radiusBounce);
        auto y = nsc.centerPos.y + std::sin(a) * (nsc.radius + nsc.radiusBounce);

        auto destRect = sourceRect;
        destRect.x = x;
        destRect.y = y;

        float rotAngle = rad2deg(a) + 90.0f;

        SDL_RenderCopyEx(renderer, nsc.nyanSheet, &sourceRect, &destRect, rotAngle, &rotCenter, SDL_FLIP_NONE);
    }

    nsc.angle = nsc.angle + nsc.angularStep;
    if (nsc.angle >= NYAN_PI2) nsc.angle -= NYAN_PI2;

    nsc.radiusBounce += nsc.radiusBounceIncrement;
    if (nsc.radiusBounce <= -nsc.radiusBounceMax || nsc.radiusBounce > nsc.radiusBounceMax)
        nsc.radiusBounceIncrement = - nsc.radiusBounceIncrement;
}

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

#ifndef NDEBUG
    log_set_level(LOG_TRACE);
#else
    log_set_level(LOG_DEBUG);
#endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER))
        nyan_sdl_fatal("SDL_Init");

#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    const auto windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL;
    auto window = SDL_CreateWindow("doompanning", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 960, windowFlags);
    if (!window)
        nyan_sdl_fatal("SDL_CreateWindow");

    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    if (!renderer)
        nyan_sdl_fatal("SDL_CreateRenderer");

    auto nyanSheet = make_nyan_texture(renderer);

    NyanSpinnyCircle nsc;
    nsc.nyanSheet = nyanSheet;
    nsc.centerPos = { 420, 420 };

    bool quit = false;

    while (!quit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                quit = true;

            if (event.type == SDL_KEYDOWN && event.key.keysym.mod & KMOD_CTRL && event.key.keysym.sym == SDLK_q)
                quit = true;
        }

        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        SDL_RenderClear(renderer);
        auto sheetDestRect = nyan_sheet_rect();
        sheetDestRect.w *= 3;
        sheetDestRect.h *= 3;
        SDL_RenderCopy(renderer, nyanSheet, nullptr, &sheetDestRect);

        auto ticks = SDL_GetTicks();
        size_t nyanSpriteIndex = (ticks / 48) % NYAN_SPRITE_COUNT;

        auto sourceRect = nyan_sprite_rect(nyanSpriteIndex);
        auto destRect = nyan_sprite_rect(0);
        destRect.w *= 3;
        destRect.h *= 3;
        destRect.y += sheetDestRect.h;
        SDL_RenderCopy(renderer, nyanSheet, &sourceRect, &destRect);

        destRect.x += destRect.w;
        destRect.y += destRect.h;

        SDL_Point centerPoint = {NYAN_SPRITE_WIDTH, NYAN_SPRITE_HEIGHT};
        double angle = (ticks / 4) % 360;
        SDL_RenderCopyEx(renderer, nyanSheet, &sourceRect, &destRect, angle, &centerPoint, SDL_FLIP_NONE);

        do_circle_nyan_step(renderer, nsc);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(nyanSheet);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
