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

void app_initialize(app_config_t *config);
void app_terminate(void);
void app_run(frame_processor_t processor);
