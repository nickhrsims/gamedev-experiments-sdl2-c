#pragma once

#include "entity.h"

typedef enum { LEFT_PADDLE, RIGHT_PADDLE } paddle_identifier_t;

/**
 * Configure
 */
void paddle_configure(entity_t *paddle, aabb_t *field,
                      paddle_identifier_t identifier);

/**
 * Initialize new paddle.
 */
entity_t *paddle_init(aabb_t *field, paddle_identifier_t identifier);
