#pragma once

#include "entity.h"

/**
 * Configure `ball` properties based on playing `field`.
 */
void ball_configure(entity_t *ball, aabb_t *field);

/**
 * Reverse the current direction of the ball.
 */
void ball_reverse_direction(entity_t *ball);
