#include <SDL2/SDL.h>

#include "graphics.h"

// -----------------------------------------------------------------------------
// Static SDL2 Resources
// -----------------------------------------------------------------------------
//
// NOTE: System does not support multiples of ANY SDL2 base resources.

static SDL_Renderer *renderer = NULL;

void graphics_init(SDL_Window *window) {
    // No use for variable index or flags.
    static unsigned char const RENDERER_INDEX = 0;
    static unsigned char const RENDERER_FLAGS = 0;

    renderer = SDL_CreateRenderer(window, RENDERER_INDEX, RENDERER_FLAGS);
}

/**
 * Clear the screen.
 */
void graphics_clear(void) {
    SDL_RenderClear(renderer);
    return;
}

/**
 * Display all drawn entities.
 */
void graphics_show(void) {
    SDL_RenderPresent(renderer);
    return;
}

/**
 * Set entity draw color.
 */
void graphics_set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    return;
}

/**
 * Reset entity draw color (set color to black).
 */
void graphics_reset_color(void) {
    graphics_set_color(0, 0, 0, 0);
    return;
}

/**
 * Draw specific entity.
 */
static void draw_entity(entity_t *e) {
    aabb_t rect;
    entity_get_aabb(e, &rect);
    SDL_RenderFillRect(renderer, &rect);
    return;
}

/**
 * Draw all entities of given game instance.
 */
void graphics_draw_entities(size_t entity_count, entity_t *entity_pool[entity_count]) {
    for (size_t entity_num = 0; entity_num < entity_count; entity_num++) {
        draw_entity(entity_pool[entity_num]);
    }
}

/**
 * Draw text.
 */
void graphics_draw_text(TTF_Font *font, char *str, int x, int y) {
    SDL_Surface *surface =
        TTF_RenderText_Solid(font, str, (SDL_Color){255, 255, 255, 255});

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){x, y, surface->w, surface->h});
}
