#include "field.h"
#include "entities/entity.h"

/**
 * Check of `subject` has crossed left threshold of `field`.
 */
bool field_is_subject_in_left_goal(aabb_t *field, entity_t *subject) {
    aabb_t entity_box;
    entity_get_aabb(subject, &entity_box);
    return aabb_is_beyond_edge(&entity_box, field, AABB_LEFT_EDGE);
}

/**
 * Check of `subject` has crossed left threshold of `field`.
 */
bool field_is_subject_in_right_goal(aabb_t *field, entity_t *subject) {
    aabb_t entity_box;
    entity_get_aabb(subject, &entity_box);
    return aabb_is_beyond_edge(&entity_box, field, AABB_RIGHT_EDGE);
}
