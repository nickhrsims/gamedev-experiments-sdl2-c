#include <stdlib.h>

#include "ball.h"
#include "entities/entity.h"

// --- Size Ratio (Larger value means smaller size)
//     (40 ~= (640 > 480) -> 640/40 = size 16 ball)
#define BALL_SIZE_RATIO 40

// --- Velocity Scale (How fast does the ball move?)
#define BALL_VELOCITY_SCALE 300

static void update(entity_t *ball, float delta) {
    ball->x += (int)(ball->vx * delta);
    ball->y += (int)(ball->vy * delta);
}

/**
 * Configure a pre-allocated ball.
 */
void ball_configure(entity_t *ball, aabb_t *field) {
    // --- Position
    int field_center_x = (field->x + field->w) / 2;
    int field_center_y = (field->y + field->h) / 2;
    entity_set_center_position(ball, field_center_x, field_center_y);

    // --- Size
    int largest_field_axis = field->w >= field->h ? field->w : field->h;
    int scaled_size        = largest_field_axis / BALL_SIZE_RATIO;
    entity_set_size(ball, scaled_size, scaled_size); // ball is square

    // --- Velocity
    int random_vx = floor(((double)rand() / (double)RAND_MAX) * BALL_VELOCITY_SCALE);
    int random_vy = floor(((double)rand() / (double)RAND_MAX) * BALL_VELOCITY_SCALE);
    entity_set_velocity(ball, random_vx, random_vy);

    ball->update = update;
}

/**
 * Reverse the current direction of the ball.
 */
void ball_reverse_direction(entity_t *ball) {
    int vx, vy;
    entity_get_velocity(ball, &vx, &vy);
    entity_set_velocity(ball, -vx, -vy);
}
