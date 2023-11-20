#include <SDL2/SDL.h>

#include "game.h"

/**
 * Clear the screen.
 */
void graphics_clear(game_t *game) {
    SDL_RenderClear(game->sdl.renderer);
    return;
}

/**
 * Display all drawn entities.
 */
void graphics_show(game_t *game) {
    SDL_RenderPresent(game->sdl.renderer);
    return;
}

/**
 * Set entity draw color.
 */
void graphics_set_color(game_t *game, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(game->sdl.renderer, r, g, b, a);
    return;
}

/**
 * Reset entity draw color (set color to black).
 */
void graphics_reset_color(game_t *game) {
    graphics_set_color(game, 0, 0, 0, 0);
    return;
}

/**
 * Draw specific entity.
 */
static void draw_entity(game_t *game, entity_t *e) {
    SDL_RenderFillRect(game->sdl.renderer,
                       &(SDL_Rect){.x = e->x, .y = e->y, .w = e->w, .h = e->h});
    return;
}

/**
 * Draw all entities of given game instance.
 */
void graphics_draw_entities(game_t *game) {
    draw_entity(game, &game->ball);
    draw_entity(game, &game->left_paddle);
    draw_entity(game, &game->right_paddle);
}

/**
 * Draw text.
 */
void graphics_draw_text(game_t *game, char *str, int16_t x, int16_t y) {
    SDL_Surface *surface =
        TTF_RenderText_Solid(game->sdl.font, str, (SDL_Color){255, 255, 255, 255});

    SDL_Texture *texture = SDL_CreateTextureFromSurface(game->sdl.renderer, surface);

    SDL_RenderCopy(game->sdl.renderer, texture, NULL,
                   &(SDL_Rect){x, y, surface->w, surface->h});
}
