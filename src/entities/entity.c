#include <stdlib.h>

#include "aabb.h"
#include "entity.h"

/**
 * Create a new entity.
 */
entity_t *entity_init(void) {
    entity_t *e = malloc(sizeof(entity_t));
    return e;
}

/**
 * Destroy an existing entity.
 */
void entity_term(entity_t *e) { free(e); }

/**
 * Get the position of a given entity.
 */
void entity_get_position(entity_t *e, int *x, int *y) {
    *x = e->x;
    *y = e->y;
}

/**
 * Set the position of a given entity.
 */
void entity_set_position(entity_t *e, int x, int y) {
    e->x = x;
    e->y = y;
}

/**
 * Get the position of the entity based on it's Euclidean origin.
 */
void entity_get_center_position(entity_t *e, int *x, int *y) {
    *x = (e->x + e->w) / 2;
    *y = (e->y + e->h) / 2;
}

/**
 * Set the position of the entity based on Euclidean origin.
 */
void entity_set_center_position(entity_t *e, int x, int y) {
    e->x = x - (e->w / 2);
    e->y = y - (e->h / 2);
}

/**
 * Get the width/height of a given entity.
 */
void entity_get_size(entity_t *e, int *w, int *h) {
    *w = e->w;
    *h = e->h;
}
/**
 * Get Set the width/height of a given entity.
 */
void entity_set_size(entity_t *e, int w, int h) {
    e->w = w;
    e->h = h;
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
 * Populate an AABB with entity parameters.
 */
void entity_get_aabb(entity_t *e, aabb_t *aabb) {
    aabb->x = e->x;
    aabb->y = e->y;
    aabb->w = e->w;
    aabb->h = e->h;
}

/**
 * Populate entity parameters from an AABB.
 */
void entity_set_aabb(entity_t *e, aabb_t *aabb) {
    e->x = aabb->x;
    e->y = aabb->y;
    e->w = aabb->w;
    e->h = aabb->h;
}

/**
 * Is entity `e` beyond box `boundary` of specific `edge`.
 */
bool entity_is_beyond_edge(entity_t *e, aabb_t *boundary, edge_t edge) {
    aabb_t entity_box;
    entity_get_aabb(e, &entity_box);
    return aabb_is_beyond_edge(&entity_box, boundary, edge);
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
