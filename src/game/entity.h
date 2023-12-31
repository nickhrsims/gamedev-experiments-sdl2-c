#pragma once

#include <stdint.h>

#include "aabb.h"

typedef enum { DIR_LEFT, DIR_UP, DIR_RIGHT, DIR_DOWN } direction_t;

typedef struct entity_s {
  aabb_t transform;
  int vx;
  int vy;
  void (*update)(struct entity_s *self, float delta);
  void (*destroy)(struct entity_s *self);
  void (*collide)(struct entity_s *self, struct entity_s *collider,
                  aabb_edge_t edge);
  void (*out_of_bounds)(struct entity_s *self, aabb_edge_t edge);
  void *data;
} entity_t;

entity_t *entity_init(void);
void entity_term(entity_t *entity);

void entity_get_velocity(entity_t *entity, int *vx, int *vy);
void entity_set_velocity(entity_t *entity, int vx, int vy);

void entity_set_direction(entity_t *e, direction_t dir);
