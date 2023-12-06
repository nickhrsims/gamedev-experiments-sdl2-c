#pragma once

#include "SDL_ttf.h"

#include "entities/entity.h"

void graphics_init(SDL_Window *window);

/**
 * Clear the screen.
 */
void graphics_clear(void);

/**
 * Display all drawn entities.
 */
void graphics_show(void);

/**
 * Set entity draw color.
 */
void graphics_set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/**
 * Reset entity draw color (set color to black).
 */
void graphics_reset_color(void);

/**
 * Draw specific entity.
 */
void graphics_draw_entities(size_t entity_count,
                            entity_t *entity_pool[entity_count]);

/*
 * Draw text.
 */
void graphics_draw_text(TTF_Font *font, char *str, int x, int y);
