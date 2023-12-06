#pragma once

#include "aabb.h"
#include "entities/entity.h"

/**
 * Check of `subject` has crossed left threshold of `field`.
 */
bool field_is_subject_in_left_goal(aabb_t *field, entity_t *subject);

/**
 * Check of `subject` has crossed left threshold of `field`.
 */
bool field_is_subject_in_right_goal(aabb_t *field, entity_t *subject);
