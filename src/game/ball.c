#include <math.h>
#include <stdlib.h>

#include "aabb.h"
#include "alloc.h"
#include "ball.h"
#include "entity.h"
#include "log.h"

// --- Size Ratio (Larger value means smaller size)
//     (40 ~= (640 > 480) -> 640/40 = size 16 ball)
#define BALL_SIZE_RATIO 40

// --- Velocity Scale (How fast does the ball move?)
#define BALL_VELOCITY_START     300
#define BALL_VELOCITY_INCREMENT 20

typedef struct ball_data_s {
    unsigned short speed;
} ball_data_t;

static void update(entity_t *ball, float delta) {
    ball->transform.x += (int)(ball->vx * delta);
    ball->transform.y += (int)(ball->vy * delta);
}

double get_normalized_vertical_difference(entity_t *self, entity_t *collider) {
    double vertical_difference   = (double)(self->transform.y - collider->transform.y);
    double normalized_difference = vertical_difference / collider->transform.h;
    return normalized_difference;
}

static void get_collision_vector(entity_t *ball, entity_t *collider, double *vx,
                                 double *vy) {
    double angular_scalar = get_normalized_vertical_difference(ball, collider);
    double phi            = (angular_scalar - 0.5) * 90 * M_PI / 180;

    *vx = cos(phi);
    *vy = sin(phi);
}

static void collide_with_paddle(entity_t *self, entity_t *paddle, aabb_edge_t edge) {
    double vx, vy;
    ball_data_t *data = self->data;
    data->speed *= 1.1;

    get_collision_vector(self, paddle, &vx, &vy);
    entity_set_velocity(self, floor(vx * data->speed), floor(vy * data->speed));

    // It is already moving right.
    if (edge == AABB_RIGHT_EDGE) {
        entity_set_direction(self, DIR_LEFT);
    }
}

static void collide(entity_t *self, entity_t *collider, aabb_edge_t edge) {
    switch (edge) {
    case AABB_LEFT_EDGE:
        collide_with_paddle(self, collider, edge);
        break;
    case AABB_RIGHT_EDGE:
        collide_with_paddle(self, collider, edge);
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
 * Get a unit vector with clamped stochastic angular possiblities.
 *
 * Vector angle is stochastically determined, but clamped between 10° - 90° (degrees),
 * as well as stochastically projected into a quadrant.
 *
 * The result is an angle that is predominantly pointing to the left or right,
 * but never "exactly" left or right without some skew. This is used to
 * determine a random angle for the ball that is moving "mostly" towards either
 * paddle, either slightly up or slight down, but never without some degree of
 * rotation.
 *
 */
static void get_clamped_stochastic_vector(double *vx, double *vy) {
    double degrees = 80 * ((double)rand() / (double)RAND_MAX - 0.5) +
                     (10 * ((double)rand() / (double)RAND_MAX));
    double radians = degrees * M_PI / 180;

    int x_dir = ((float)rand() / (float)RAND_MAX) < 0.5 ? -1 : 1;
    int y_dir = ((float)rand() / (float)RAND_MAX) < 0.5 ? -1 : 1;

    *vx = cos(radians) * x_dir;
    *vy = sin(radians) * y_dir;
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
    if (!ball->data) {
        ball->data = new (ball_data_t);
    }

    ball_data_t *data = ball->data;
    data->speed       = BALL_VELOCITY_START;

    double vx, vy;
    get_clamped_stochastic_vector(&vx, &vy);
    entity_set_velocity(ball, (int)floor(vx * data->speed),
                        (int)floor(vy * data->speed));

    // --- Polymorphic Properties
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
