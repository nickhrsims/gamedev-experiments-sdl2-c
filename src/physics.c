#include <SDL2/SDL.h>

#include "game.h"
#include "physics.h"

/**
 * Move the ball based on velocity.
 */
void physics_move_entity(entity_t *e, float delta) {
    // Casting prevents bit overflow observed during testing
    e->x += (int)(e->vx * delta);
    e->y += (int)(e->vy * delta);
}

/**
 * Check collision between ball and edges.
 */
void physics_check_collision_with_edges(game_t *game) {
    entity_t *ball = &game->ball;

    // TODO: Use of config does not support resize.
    uint16_t left_wall   = 0;
    uint16_t top_wall    = 0;
    uint16_t right_wall  = game->config.window_width;
    uint16_t bottom_wall = game->config.window_height;

    if (ball->x <= left_wall) {
        ball->vx = abs(ball->vx);
    }

    if (ball->y <= top_wall) {
        ball->vy = abs(ball->vy);
    }

    if (ball->x + ball->w >= right_wall) {
        ball->vx = -abs(ball->vx);
    }

    if (ball->y + ball->h >= bottom_wall) {
        ball->vy = -abs(ball->vy);
    }
}

uint8_t _physics_is_colliding(entity_t *a, entity_t *b) {
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

/**
 * Check collision between ball and paddles.
 */
void physics_check_collision_with_paddles(game_t *game) {

    entity_t *b  = &game->ball;
    entity_t *lp = &game->left_paddle;
    entity_t *rp = &game->right_paddle;

    if (_physics_is_colliding(b, lp)) {
        b->vx *= -1;
    } else if (_physics_is_colliding(b, rp)) {
        b->vx *= -1;
    }

    return;
}
