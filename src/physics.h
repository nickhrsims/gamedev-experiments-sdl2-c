#pragma once

#include "entities/entity.h"

/**
 * Move entities based on their current position, velocity, and time delta.
 */
void physics_move_entities(size_t entity_count,
                           entity_t *entity_pool[entity_count], float delta);
