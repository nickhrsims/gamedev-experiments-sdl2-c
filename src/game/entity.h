#pragma once

#include <stdint.h>

#include "aabb.h"

typedef enum { DIR_LEFT, DIR_UP, DIR_RIGHT, DIR_DOWN } direction_t;

typedef struct entity_s {
  int x;
  int y;
  int w;
  int h;
  int vx;
  int vy;
  void (*update)(struct entity_s *, float);
  void (*destroy)(struct entity_s *);
  void (*collide)(struct entity_s *, aabb_edge_t edge);
  void (*out_of_bounds)(struct entity_s *, aabb_edge_t edge);
  void *data;
} entity_t;

entity_t *entity_init(void);
void entity_term(entity_t *entity);

void entity_get_position(entity_t *entity, int *x, int *y);
void entity_set_position(entity_t *entity, int x, int y);

void entity_get_center_position(entity_t *entity, int *x, int *y);
void entity_set_center_position(entity_t *entity, int x, int y);

void entity_get_size(entity_t *entity, int *w, int *h);
void entity_set_size(entity_t *entity, int w, int h);

void entity_get_velocity(entity_t *entity, int *vx, int *vy);
void entity_set_velocity(entity_t *entity, int vx, int vy);

void entity_get_aabb(entity_t *entity, aabb_t *aabb);
void entity_set_aabb(entity_t *entity, aabb_t *aabb);

/**
 * Like it's AABB variant, but wraps an entity.
 */
bool entity_is_beyond_edge(entity_t *entity, aabb_t *box, aabb_edge_t edge);

void entity_set_direction(entity_t *e, direction_t dir);
