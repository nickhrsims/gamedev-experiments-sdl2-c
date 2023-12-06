#include "aabb.h"

/**
 * Return `true` if `inner` is beyond `outer` for given `edge`.
 *
 * Can be composed to check wall collision in rectangular game field.
 */
bool aabb_is_beyond_edge(aabb_t *inner, aabb_t *outer, edge_t edge) {

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
        return 0;
    }
}

/**
 * Return `true` if `a` overlaps with `b`.
 */
bool aabb_is_intersecting(aabb_t *a, aabb_t *b) {
    int a_left   = a->x;
    int a_top    = a->y;
    int a_right  = a->x + a->w;
    int a_bottom = a->y + a->h;

    int b_left   = b->x;
    int b_top    = b->y;
    int b_right  = b->x + b->w;
    int b_bottom = b->y + b->h;

    bool is_colliding_x = a_left <= b_right && b_left <= a_right;
    bool is_colliding_y = a_top <= b_bottom && b_top <= a_bottom;

    return is_colliding_x && is_colliding_y;
}
