#pragma once

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL_ttf.h>

/**
 * Application Configuration Parameters.
 */
typedef struct {
  char *window_title;
  unsigned short window_position_x;
  unsigned short window_position_y;
  unsigned short window_width;
  unsigned short window_height;
  unsigned char window_is_fullscreen;
} app_config_t;

typedef struct {
  SDL_Window *window;
  TTF_Font *font;
} app_t;

typedef bool (*frame_processor_t)(app_t *, float);

// TODO: Redesign relationship to app allocation
void app_init(app_t *app, app_config_t *config);
void app_term(app_t *app);
void app_run(app_t *app, frame_processor_t processor);
