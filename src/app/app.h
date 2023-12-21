#pragma once

#include <stdbool.h>

#include "video.h"

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
  video_t *video;
} app_t;

typedef bool (*frame_processor_t)(app_t *, float);
typedef void (*event_processor_t)(app_t *, SDL_Event *event);

// TODO: Redesign relationship to app allocation
app_t *app_init(app_config_t *config);
void app_term(app_t *app);
void app_run(app_t *app, frame_processor_t frame_processor,
             event_processor_t event_processor);
