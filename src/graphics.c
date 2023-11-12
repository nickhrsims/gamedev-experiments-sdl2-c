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
void _graphics_draw_entity(game_t *game, entity_t *e) {
    SDL_RenderFillRect(game->sdl.renderer,
                       &(SDL_Rect){.x = e->x, .y = e->y, .w = e->w, .h = e->h});
    return;
}

/**
 * Draw all entities of given game instance.
 */
void graphics_draw_entities(game_t *game) {
    _graphics_draw_entity(game, &game->ball);
    _graphics_draw_entity(game, &game->left_paddle);
    _graphics_draw_entity(game, &game->right_paddle);
}
