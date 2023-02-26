#include "log.h"
#include <SDL.h>

void nyan_sdl_fatal(const char *const msg)
{
    log_fatal("%s: %s", msg, SDL_GetError());
    abort();
}

void nyan_sdl_error(const char *const msg)
{
    log_error("%s: %s", msg, SDL_GetError());
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

    return 0;
}
