#pragma once

#include "entities/entity.h"

typedef enum {
  COLLIDING_ON_LEFT,
  COLLIDING_ON_RIGHT,
  COLLIDING_ON_TOP,
  COLLIDING_ON_BOTTOM,
  NOT_COLLIDING
} collision_t;

/**
 * Move entities based on their current position, velocity, and time delta.
 */
void physics_move_entities(size_t entity_count,
                           entity_t *entity_pool[entity_count], float delta);

collision_t physics_get_collision(entity_t *a, entity_t *b);
