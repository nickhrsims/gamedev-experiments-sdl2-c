#include "field.h"
#include "entity.h"

/**
 * Check of `subject` has crossed left threshold of `field`.
 */
bool field_is_subject_in_left_goal(aabb_t *field, aabb_t *subject) {
    return aabb_is_beyond_edge(subject, field, AABB_LEFT_EDGE);
}

/**
 * Check of `subject` has crossed left threshold of `field`.
 */
bool field_is_subject_in_right_goal(aabb_t *field, aabb_t *subject) {
    return aabb_is_beyond_edge(subject, field, AABB_RIGHT_EDGE);
}
