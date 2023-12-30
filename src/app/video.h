#pragma once

#include "SDL_ttf.h"

typedef struct video_s video_t;

/**
 * Video System Configuration Parameters.
 */
typedef struct {
  char *window_title;
  unsigned short window_position_x;
  unsigned short window_position_y;
  unsigned short window_width;
  unsigned short window_height;
  unsigned char window_is_fullscreen;
} video_cfg_t;

/**
 * Initialize video handle.
 *
 * \param config Video configuration.
 * \returns video_t on success or NULL on error.
 * \sa video_term
 */
video_t *video_init(video_cfg_t *config);

/**
 * Terminate video handle.
 */
void video_term(video_t *video);

/**
 * Clear the screen.
 */
void video_clear(video_t *video);

/**
 * Display all drawn elements.
 */
void video_render(video_t *video);

/**
 * Set draw color.
 */
void video_set_color(video_t *video, uint8_t r, uint8_t g, uint8_t b,
                     uint8_t a);

/**
 * Reset entity draw color (set color to black).
 */
void video_reset_color(video_t *video);

/**
 * Draw an axis-aligned region on the screen.
 */
void video_draw_region(video_t *video, int x, int y, int w, int h);

/**
 * Draw text.
 * TODO: Restore font select.
 *       (can support opaque video via. app passthru API)
 */
void video_draw_text(video_t *video, char *str, int x, int y);

/**
 * Get window size.
 */
void video_get_window_size(video_t *video, int *width, int *height);
