#include "entity.h"

/**
 * Process collisions between all given entities.
 */
void collision_process(size_t entity_count,
                       entity_t *entity_pool[entity_count]);

/**
 * Process field-edge collisions for all given entities and field.
 */
void collision_out_of_bounds_process(size_t entity_count,
                                     entity_t *entity_pool[entity_count],
                                     aabb_t *field);
