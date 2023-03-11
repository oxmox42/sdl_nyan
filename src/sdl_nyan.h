#ifndef SRC_SDL_NYAN_H
#define SRC_SDL_NYAN_H

#include <SDL_render.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NYAN_SPRITE_COUNT 12u
#define NYAN_SPRITE_WIDTH 36u
#define NYAN_SPRITE_HEIGHT 26u
#define NYAN_BBP 4u

SDL_Texture *make_nyan_sprite_sheet_from_mem(SDL_Renderer *renderer);

static SDL_Rect nyan_sprite_rect(size_t index)
{
    return SDL_Rect{ static_cast<int>(NYAN_SPRITE_WIDTH * index), 0, NYAN_SPRITE_WIDTH, NYAN_SPRITE_HEIGHT};
}

static constexpr SDL_Rect nyan_sheet_rect()
{
    return { 0, 0, NYAN_SPRITE_COUNT * NYAN_SPRITE_WIDTH, NYAN_SPRITE_HEIGHT };
}

#ifdef __cplusplus
}
#endif

#endif // SRC_SDL_NYAN_H
