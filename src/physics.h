#pragma once

#include "entities/entity.h"

typedef enum {
  COLLIDING_ON_LEFT,
  COLLIDING_ON_RIGHT,
  COLLIDING_ON_TOP,
  COLLIDING_ON_BOTTOM,
  NOT_COLLIDING
} collision_t;

collision_t physics_get_collision(entity_t *a, entity_t *b);
