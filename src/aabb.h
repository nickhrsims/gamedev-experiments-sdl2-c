#pragma once

#include <stdbool.h>

#include <SDL_rect.h>

typedef SDL_Rect aabb_t;

typedef enum {
  AABB_LEFT_EDGE,
  AABB_TOP_EDGE,
  AABB_RIGHT_EDGE,
  AABB_BOTTOM_EDGE
} edge_t;

/**
 * Return `true` if `inner` is beyond `outer` for given `edge`.
 *
 * Can be composed to check wall collision in rectangular game field.
 */
bool aabb_is_beyond_edge(aabb_t *inner, aabb_t *outer, edge_t edge);

/**
 * Return `true` if `a` overlaps with `b`.
 */
bool aabb_is_intersecting(aabb_t *a, aabb_t *b);
