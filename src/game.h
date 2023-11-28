#pragma once

#include <stdint.h>

#include <SDL2/SDL.h>
#include <SDL_ttf.h>

/**
 * Game Physics Body (Ball, Paddles, etc.)
 */
typedef struct {
  int16_t x, y;
  int16_t w, h;
  int16_t vx, vy;
} entity_t;

/**
 * Player Data
 */
typedef struct {
  uint8_t score;
} player_t;

/**
 * SDL2 Resource Container
 */
typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;
} game_sdl_resource_container_t;

/**
 * Game Configuration Parameters
 */
typedef struct {
  char *window_title;
  uint16_t window_position_x;
  uint16_t window_position_y;
  uint16_t window_width;
  uint16_t window_height;
  uint8_t window_is_fullscreen;
} game_config_t;

/**
 * Game Instance Data
 */
typedef struct {
  game_sdl_resource_container_t sdl;
  game_config_t config;
  player_t player_1;
  player_t player_2;
  entity_t left_paddle;
  entity_t right_paddle;
  entity_t ball;

  uint8_t running;
} game_t;

game_t *game_init(game_config_t);

void game_loop(game_t *);
