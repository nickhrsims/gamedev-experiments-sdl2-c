#pragma once

#include "game.h"

/**
 * Clear the screen.
 */
void graphics_clear(game_t *game);

/**
 * Display all drawn entities.
 */
void graphics_show(game_t *game);

/**
 * Set entity draw color.
 */
void graphics_set_color(game_t *game, uint8_t r, uint8_t g, uint8_t b,
                        uint8_t a);

/**
 * Reset entity draw color (set color to black).
 */
void graphics_reset_color(game_t *game);

/**
 * Draw specific entity.
 */
void graphics_draw_entity(game_t *game, entity_t *e);

/**
 * Draw all entities of given game instance.
 */
void graphics_draw_entities(game_t *game);

/*
 * Draw text.
 */
void graphics_draw_text(game_t *game, char *str, int8_t x, int8_t y);
