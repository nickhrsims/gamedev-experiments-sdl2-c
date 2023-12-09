#include <stdbool.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "aabb.h"
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

static void minkowksi_aabb(aabb_t *a, aabb_t *b, aabb_t *out) {
    out->x = a->x - (b->x + b->w);
    out->y = a->y - (b->y + b->h);
    out->w = a->w + b->w;
    out->h = a->h + b->h;
}

static bool is_point_within_aabb(aabb_t *box, int x, int y) {
    int left   = box->x;
    int top    = box->y;
    int right  = box->x + box->w;
    int bottom = box->y + box->h;

    return (left < x && x < right && top < y && y < bottom);
}

collision_t physics_get_collision(entity_t *a, entity_t *b) {
    aabb_t box_a;
    aabb_t box_b;
    entity_get_aabb(a, &box_a);
    entity_get_aabb(b, &box_b);

    aabb_t minkowski_box;
    minkowksi_aabb(&box_a, &box_b, &minkowski_box);

    // If the minkowski box surrounds the origin, then a collision has occured.
    bool is_colliding = is_point_within_aabb(&minkowski_box, 0, 0);

    int left   = abs(minkowski_box.x);
    int top    = abs(minkowski_box.y);
    int right  = abs(minkowski_box.x + minkowski_box.w);
    int bottom = abs(minkowski_box.y + minkowski_box.h);

    if (is_colliding) {
        int smallest_distance = left;
        if (right < smallest_distance)
            smallest_distance = right;
        if (top < smallest_distance)
            smallest_distance = top;
        if (bottom < smallest_distance)
            smallest_distance = bottom;

        if (smallest_distance == left)
            return COLLIDING_ON_LEFT;
        if (smallest_distance == right)
            return COLLIDING_ON_RIGHT;
        if (smallest_distance == top)
            return COLLIDING_ON_TOP;
        if (smallest_distance == bottom)
            return COLLIDING_ON_BOTTOM;
    }

    return NOT_COLLIDING;
}
