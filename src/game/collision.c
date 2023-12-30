#include "collision.h"
#include "log.h"

void collision_process(size_t entity_count, entity_t *entity_pool[entity_count]) {
    aabb_t subject_box, collider_box;
    for (size_t subject_index = 0; subject_index < entity_count; subject_index++) {
        entity_t *subject = entity_pool[subject_index];
        entity_get_aabb(subject, &subject_box);
        for (size_t collider_index = 0; collider_index < entity_count;
             collider_index++) {
            if (subject_index == collider_index)
                continue;
            entity_t *collider = entity_pool[collider_index];
            entity_get_aabb(collider, &collider_box);
            aabb_edge_t intersection =
                aabb_get_intersection(&subject_box, &collider_box);
            if (intersection && subject->collide) {
                subject->collide(subject, collider, intersection);
            }
        }
    }
}

void collision_out_of_bounds_process(size_t entity_count,
                                     entity_t *entity_pool[entity_count],
                                     aabb_t *field) {
    aabb_t subject_box;
    for (size_t subject_index = 0; subject_index < entity_count; subject_index++) {
        entity_t *subject = entity_pool[subject_index];
        entity_get_aabb(subject, &subject_box);
        if (subject->out_of_bounds) {
            if (aabb_is_beyond_edge(&subject_box, field, AABB_TOP_EDGE)) {
                subject->out_of_bounds(subject, AABB_TOP_EDGE);
            } else if (aabb_is_beyond_edge(&subject_box, field, AABB_BOTTOM_EDGE)) {
                subject->out_of_bounds(subject, AABB_BOTTOM_EDGE);
            } else if (aabb_is_beyond_edge(&subject_box, field, AABB_LEFT_EDGE)) {
                subject->out_of_bounds(subject, AABB_LEFT_EDGE);
            } else if (aabb_is_beyond_edge(&subject_box, field, AABB_RIGHT_EDGE)) {
                subject->out_of_bounds(subject, AABB_RIGHT_EDGE);
            }
        }
    }
}
