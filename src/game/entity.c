#include <stdlib.h>

#include "aabb.h"
#include "alloc.h"
#include "entity.h"

/**
 * Create a new entity.
 */
entity_t *entity_init(void) {
    entity_t *e = new (entity_t);
    return e;
}

/**
 * Destroy an existing entity.
 */
void entity_term(entity_t *e) {
    if (e->destroy)
        e->destroy(e);
    free(e);
}

/**
 * Get the velocity of a given entity.
 */
void entity_get_velocity(entity_t *e, int *vx, int *vy) {
    *vx = e->vx;
    *vy = e->vy;
}

/**
 * Set the velocity of a given entity.
 */
void entity_set_velocity(entity_t *e, int vx, int vy) {
    e->vx = vx;
    e->vy = vy;
}

/**
 * Change entity direction (one axis at a time).
 *
 * Can be composed to "bounce" and entity off of something.
 */
void entity_set_direction(entity_t *e, direction_t dir) {
    switch (dir) {
    case DIR_LEFT:
        e->vx = -abs(e->vx);
        break;
    case DIR_UP:
        e->vy = -abs(e->vy);
        break;
    case DIR_RIGHT:
        e->vx = abs(e->vx);
        break;
    case DIR_DOWN:
        e->vy = abs(e->vy);
        break;
    default:
        break;
    }
}
