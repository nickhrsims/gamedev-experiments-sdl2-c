#include "aabb.h"

/**
 * Return `true` if `inner` is beyond `outer` for given `edge`.
 *
 * Can be composed to check wall collision in rectangular game field.
 */
bool aabb_is_beyond_edge(aabb_t *inner, aabb_t *outer, aabb_edge_t edge) {

    int outer_border = 0;
    int inner_border = 0;

    switch (edge) {
    case AABB_LEFT_EDGE:
        outer_border = outer->x;
        inner_border = inner->x;
        return outer_border >= inner_border;
    case AABB_TOP_EDGE:
        outer_border = outer->y;
        inner_border = inner->y;
        return outer_border >= inner_border;
    case AABB_RIGHT_EDGE:
        outer_border = outer->x + outer->w;
        inner_border = inner->x + inner->w;
        return outer_border <= inner_border;
    case AABB_BOTTOM_EDGE:
        outer_border = outer->y + outer->h;
        inner_border = inner->y + inner->h;
        return outer_border <= inner_border;
    default:
        return false;
    }
}

static void minkowksi_difference(aabb_t *a, aabb_t *b, aabb_t *out) {
    out->x = a->x - (b->x + b->w);
    out->y = a->y - (b->y + b->h);
    out->w = a->w + b->w;
    out->h = a->h + b->h;
}

static bool is_point_within_aabb(aabb_t *aabb, int x, int y) {
    int left   = aabb->x;
    int top    = aabb->y;
    int right  = aabb->x + aabb->w;
    int bottom = aabb->y + aabb->h;

    return (left < x && x < right && top < y && y < bottom);
}

aabb_edge_t aabb_get_intersection(aabb_t *a, aabb_t *b) {
    aabb_t c;
    minkowksi_difference(a, b, &c);

    // If the minkowski box surrounds the origin, then a collision has occured.
    bool is_colliding = is_point_within_aabb(&c, 0, 0);

    int left   = abs(c.x);
    int top    = abs(c.y);
    int right  = abs(c.x + c.w);
    int bottom = abs(c.y + c.h);

    if (is_colliding) {
        int smallest_distance = left;
        if (right < smallest_distance)
            smallest_distance = right;
        if (top < smallest_distance)
            smallest_distance = top;
        if (bottom < smallest_distance)
            smallest_distance = bottom;

        if (smallest_distance == left)
            return AABB_LEFT_EDGE;
        if (smallest_distance == right)
            return AABB_RIGHT_EDGE;
        if (smallest_distance == top)
            return AABB_TOP_EDGE;
        if (smallest_distance == bottom)
            return AABB_BOTTOM_EDGE;
    }

    return AABB_NO_EDGE; // Not intersecting
}

void aabb_set_center_position(aabb_t *aabb, int x, int y) {
    aabb->x = x + (aabb->w / 2);
    aabb->y = y + (aabb->h / 2);
}
