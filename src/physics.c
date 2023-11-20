#include <SDL2/SDL.h>

#include "game.h"
#include "physics.h"

typedef enum { WALL_LEFT, WALL_TOP, WALL_RIGHT, WALL_BOTTOM } wall_t;
typedef enum { DIR_LEFT, DIR_UP, DIR_RIGHT, DIR_DOWN } direction_t;

// ---------------------------------------------------------
// Motion
// ---------------------------------------------------------

/**
 * Move the ball based on velocity.
 */
static void apply_motion(entity_t *e, float delta) {
    // Casting prevents bit overflow observed during testing
    e->x += (int)(e->vx * delta);
    e->y += (int)(e->vy * delta);
}

static void set_direction(entity_t *e, direction_t dir) {
    switch (dir) {
    case DIR_LEFT:
        e->vx = -abs(e->vx);
        break;
    case DIR_UP:
        e->vy = -abs(e->vy);
        break;
    case DIR_RIGHT:
        e->vx = abs(e->vx);
        break;
    case DIR_DOWN:
        e->vy = abs(e->vy);
        break;
    default:
        break;
    }
}

// ---------------------------------------------------------
// Collision
// ---------------------------------------------------------

static uint8_t is_colliding_with_wall(game_t *game, entity_t *e, wall_t wall) {
    int16_t wall_edge   = 0;
    int16_t entity_edge = 0;
    switch (wall) {
    case WALL_LEFT:
        wall_edge   = 0;
        entity_edge = e->x;
        return wall_edge >= entity_edge;
    case WALL_TOP:
        wall_edge   = 0;
        entity_edge = e->y;
        return wall_edge >= entity_edge;
    case WALL_RIGHT:
        wall_edge   = game->config.window_width;
        entity_edge = e->x + e->w;
        return wall_edge <= entity_edge;
    case WALL_BOTTOM:
        wall_edge   = game->config.window_height;
        entity_edge = e->y + e->h;
        return wall_edge <= entity_edge;
    default:
        return 0;
    }
}

uint8_t is_colliding_with_entity(entity_t *a, entity_t *b) {
    int16_t a_left   = a->x;
    int16_t a_top    = a->y;
    int16_t a_right  = a->x + a->w;
    int16_t a_bottom = a->y + a->h;

    int16_t b_left   = b->x;
    int16_t b_top    = b->y;
    int16_t b_right  = b->x + b->w;
    int16_t b_bottom = b->y + b->h;

    uint8_t is_colliding_x = a_left <= b_right && b_left <= a_right;
    uint8_t is_colliding_y = a_top <= b_bottom && b_top <= a_bottom;

    return is_colliding_x && is_colliding_y;
}

static void apply_collision_to_ball(game_t *game) {
    entity_t *ball = &game->ball;
    entity_t *lpad = &game->left_paddle;
    entity_t *rpad = &game->right_paddle;

    if (is_colliding_with_wall(game, ball, WALL_TOP)) {
        set_direction(ball, DIR_DOWN);
    } else if (is_colliding_with_wall(game, ball, WALL_BOTTOM)) {
        set_direction(ball, DIR_UP);
    }

    if (is_colliding_with_entity(ball, lpad)) {
        set_direction(ball, DIR_RIGHT);
    } else if (is_colliding_with_entity(ball, rpad)) {
        set_direction(ball, DIR_LEFT);
    }
}

static void apply_collision_to_paddles(game_t *game) {
    entity_t *lpad = &game->left_paddle;
    entity_t *rpad = &game->right_paddle;

    if (is_colliding_with_wall(game, lpad, WALL_TOP)) {
        set_direction(lpad, DIR_DOWN);
    } else if (is_colliding_with_wall(game, lpad, WALL_BOTTOM)) {
        set_direction(lpad, DIR_UP);
    }

    if (is_colliding_with_wall(game, rpad, WALL_TOP)) {
        set_direction(rpad, DIR_DOWN);
    } else if (is_colliding_with_wall(game, rpad, WALL_BOTTOM)) {
        set_direction(rpad, DIR_UP);
    }
}

// ---------------------------------------------------------
// Application
// ---------------------------------------------------------

void physics_apply(game_t *game, float delta) {
    entity_t *ball = &game->ball;
    entity_t *lpad = &game->left_paddle;
    entity_t *rpad = &game->right_paddle;

    apply_collision_to_ball(game);
    apply_collision_to_paddles(game);

    apply_motion(ball, delta);
    apply_motion(lpad, delta);
    apply_motion(rpad, delta);
}
