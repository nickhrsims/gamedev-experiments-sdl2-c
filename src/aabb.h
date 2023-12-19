#pragma once

#include <stdbool.h>

#include <SDL_rect.h>

typedef SDL_Rect aabb_t;

typedef enum {
  AABB_NO_EDGE,
  AABB_LEFT_EDGE,
  AABB_TOP_EDGE,
  AABB_RIGHT_EDGE,
  AABB_BOTTOM_EDGE
} aabb_edge_t;

/**
 * Return `true` if `inner` is beyond `outer` for given `edge`.
 *
 * Can be composed to check wall collision in rectangular game field.
 */
bool aabb_is_beyond_edge(aabb_t *inner, aabb_t *outer, aabb_edge_t edge);

/**
 * Get edge where `a` most closely overlaps with `b`, or `false` if no overlap.
 */
aabb_edge_t aabb_get_intersection(aabb_t *a, aabb_t *b);
