#include <stdlib.h>

#include "aabb.h"
#include "ball.h"
#include "entity.h"

// --- Size Ratio (Larger value means smaller size)
//     (40 ~= (640 > 480) -> 640/40 = size 16 ball)
#define BALL_SIZE_RATIO 40

// --- Velocity Scale (How fast does the ball move?)
#define BALL_VELOCITY_SCALE 300

static void update(entity_t *ball, float delta) {
    ball->transform.x += (int)(ball->vx * delta);
    ball->transform.y += (int)(ball->vy * delta);
}

static void collide(entity_t *self, entity_t *collider, aabb_edge_t edge) {
    (void)collider;
    switch (edge) {
    case AABB_LEFT_EDGE:
        entity_set_direction(self, DIR_RIGHT);
        break;
    case AABB_RIGHT_EDGE:
        entity_set_direction(self, DIR_LEFT);
        break;
    case AABB_TOP_EDGE:
        entity_set_direction(self, DIR_DOWN);
        break;
    case AABB_BOTTOM_EDGE:
        entity_set_direction(self, DIR_UP);
        break;
    case AABB_NO_EDGE:
        break;
    default:
        break;
    }
}

static void out_of_bounds(entity_t *self, aabb_edge_t edge) {
    switch (edge) {
    case AABB_TOP_EDGE:
        entity_set_direction(self, DIR_DOWN);
        break;
    case AABB_BOTTOM_EDGE:
        entity_set_direction(self, DIR_UP);
        break;
    default:
        break;
    }
}

/**
 * Configure a pre-allocated ball.
 */
void ball_configure(entity_t *ball, aabb_t *field) {
    // --- Position
    int field_center_x = (field->x + field->w) / 2;
    int field_center_y = (field->y + field->h) / 2;
    aabb_set_center_position(&ball->transform, field_center_x, field_center_y);

    // --- Size
    int largest_field_axis = field->w >= field->h ? field->w : field->h;
    int scaled_size        = largest_field_axis / BALL_SIZE_RATIO;
    ball->transform.w      = scaled_size;
    ball->transform.h      = scaled_size;

    // --- Velocity
    int random_vx = floor(((double)rand() / (double)RAND_MAX) * BALL_VELOCITY_SCALE);
    int random_vy = floor(((double)rand() / (double)RAND_MAX) * BALL_VELOCITY_SCALE);
    entity_set_velocity(ball, random_vx, random_vy);

    ball->update        = update;
    ball->collide       = collide;
    ball->out_of_bounds = out_of_bounds;
}

entity_t *ball_init(aabb_t *field) {
    entity_t *ball = entity_init();
    ball_configure(ball, field);
    return ball;
}

/**
 * Reverse the current direction of the ball.
 */
void ball_reverse_direction(entity_t *ball) {
    int vx, vy;
    entity_get_velocity(ball, &vx, &vy);
    entity_set_velocity(ball, -vx, -vy);
}
