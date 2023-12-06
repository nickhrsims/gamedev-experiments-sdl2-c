#include <stdbool.h>

#include <SDL2/SDL.h>

#include "physics.h"

// ---------------------------------------------------------
// Motion
// ---------------------------------------------------------

/**
 * Move single entity.
 */
static void move_entity(entity_t *e, float delta) {
    // Casting prevents bit overflow observed during testing
    // TODO: Confirm this is still a problem with current type changes.
    e->x += (int)(e->vx * delta);
    e->y += (int)(e->vy * delta);
}

/**
 * Move entities based on their current position, velocity, and time delta.
 */
void physics_move_entities(size_t entity_count, entity_t *entity_pool[entity_count],
                           float delta) {
    for (size_t entity_num = 0; entity_num < entity_count; entity_num++) {
        entity_t *e = entity_pool[entity_num];
        move_entity(e, delta);
    }
}
