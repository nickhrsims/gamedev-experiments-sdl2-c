#include <SDL2/SDL.h>

#include <log.h>

#include "app.h"
#include "entities/ball.h"
#include "entities/entity.h"
#include "entities/paddle.h"
#include "field.h"
#include "fsm.h"
#include "game.h"
#include "graphics.h"
#include "physics.h"
#include "player.h"

// -----------------------------------------------------------------------------
// Game Components
// -----------------------------------------------------------------------------
static player_t player_1     = {0}; // Players do not need configuration.
static player_t player_2     = {0}; // Just init to zero and away we go!
static entity_t ball         = {0};
static entity_t left_paddle  = {0};
static entity_t right_paddle = {0};
static aabb_t field          = {0};
static bool is_game_running  = false;

// -----------------------------------------------------------------------------
// State Machine
// -----------------------------------------------------------------------------

// State Options
enum game_state_enum {
    INIT_STATE = 1,
    PLAYING_STATE,
    GAME_OVER_STATE,
    TERM_STATE,
    STATE_COUNT,
};

// Trigger Options
enum game_trigger_enum {
    INIT_DONE_TRIGGER = 1,
    QUIT_GAME_TRIGGER,
    GAME_OVER_TRIGGER,
    ALWAYS_TRIGGER,
    TRIGGER_COUNT,
};

// Current State
static unsigned int game_state = INIT_STATE;

// Transition Table
static unsigned int fsm[STATE_COUNT][TRIGGER_COUNT] = {
    [INIT_STATE] =
        {
            [INIT_DONE_TRIGGER] = PLAYING_STATE,
        },
    [PLAYING_STATE] =
        {
            [GAME_OVER_TRIGGER] = GAME_OVER_STATE,
            [QUIT_GAME_TRIGGER] = TERM_STATE,
        },
    [GAME_OVER_STATE] =
        {
            [ALWAYS_TRIGGER] = TERM_STATE,
        },
    [TERM_STATE] =
        {
            [ALWAYS_TRIGGER] = TERM_STATE,

        },
};

// Transition Wrapper Function
static void trigger(unsigned int trigger_id) {
    fsm_send(STATE_COUNT, TRIGGER_COUNT, fsm, trigger_id, &game_state);
}

/**
 * Respond to a player receiving a goal.
 *
 * NOTE: Very rudimentary implementation.
 */
static void handle_goal(player_t *player) {
    player_inc_score(player);
    ball_configure(&ball, &field);
}
// -----------------------------------------------------------------------------
// Gameplay Loop Components
// -----------------------------------------------------------------------------

/**
 * Input processing block.
 */
static void do_inputs_process(void) {

    SDL_Event event;
    SDL_PollEvent(&event);

    uint8_t const *kb = SDL_GetKeyboardState(NULL);

    // Quit Game
    if (event.type == SDL_QUIT || kb[SDL_SCANCODE_ESCAPE]) {
        trigger(QUIT_GAME_TRIGGER);
    }

    // TODO: Move into keyboard/actions abstraction

    // Left Paddle (A: UP, Z: DOWN)
    if (kb[SDL_SCANCODE_A]) {
        entity_set_velocity(&left_paddle, 0, -200);
    } else if (kb[SDL_SCANCODE_Z]) {
        entity_set_velocity(&left_paddle, 0, 200);
    } else {
        entity_set_velocity(&left_paddle, 0, 0);
    }

    // Right Paddle (K: UP, M: DOWN)
    if (kb[SDL_SCANCODE_K]) {
        entity_set_velocity(&right_paddle, 0, -200);
    } else if (kb[SDL_SCANCODE_M]) {
        entity_set_velocity(&right_paddle, 0, 200);
    } else {
        entity_set_velocity(&right_paddle, 0, 0);
    }

    return;
}

/**
 * Graphics processing block.
 */
static void do_graphics_process(app_t *app) {
    char p1_score_str[3];
    char p2_score_str[3];

    // TODO: Abstract into itoa-like func for players
    snprintf(p1_score_str, 3, "%d", player_get_score(&player_1));
    snprintf(p2_score_str, 3, "%d", player_get_score(&player_2));

    graphics_clear();
    graphics_set_color(255, 255, 255, 255);
    graphics_draw_entities(3, (entity_t *[3]){&ball, &left_paddle, &right_paddle});
    graphics_draw_text(app->font, p1_score_str, (field.x + field.w) / 2 - 48, 16);
    graphics_draw_text(app->font, p2_score_str, (field.x + field.w) / 2 + 48, 16);
    graphics_reset_color();
    graphics_show();

    return;
}

static void check_goal_conditions(void) {

    static unsigned char const winning_score = 5;

    // Is the ball in the left goal?
    if (field_is_subject_in_left_goal(&field, &ball)) {
        // player 2 gets the point
        handle_goal(&player_2);
    }

    // Is the ball in the right goal?
    else if (field_is_subject_in_right_goal(&field, &ball)) {
        // player 1 gets the point
        handle_goal(&player_1);
    }

    // TODO: Alter the triggers to relate which player won.

    // Did player 1 win?
    if (player_get_score(&player_1) >= winning_score) {
        trigger(GAME_OVER_TRIGGER);
    }

    // Did player 2 win?
    else if (player_get_score(&player_2) >= winning_score) {
        trigger(GAME_OVER_TRIGGER);
    }
}

// -------------------------------------
// Per-State Processing Blocks
// -------------------------------------

/**
 * Processing block when STATE == INITIALIZING
 */
static void do_initialize_game(app_t *app) {
    // --- Graphics System
    graphics_init(app->window);

    // --- Field Configuration
    int window_width, window_height;
    SDL_GetWindowSize(app->window, &window_width, &window_height);
    field.w = window_width;
    field.h = window_height;

    // --- Entity Configuration
    ball_configure(&ball, &field);
    paddle_configure(&left_paddle, &field, LEFT_PADDLE);
    paddle_configure(&right_paddle, &field, RIGHT_PADDLE);

    trigger(INIT_DONE_TRIGGER);
    log_debug("Initialization Complete");
}

static void apply_collision_to_ball(void) {

    aabb_t ball_box;
    aabb_t lpad_box;
    aabb_t rpad_box;

    entity_get_aabb(&ball, &ball_box);
    entity_get_aabb(&left_paddle, &lpad_box);
    entity_get_aabb(&right_paddle, &rpad_box);

    if (aabb_is_beyond_edge(&ball_box, &field, AABB_TOP_EDGE)) {
        entity_set_direction(&ball, DIR_DOWN);
    } else if (aabb_is_beyond_edge(&ball_box, &field, AABB_BOTTOM_EDGE)) {
        entity_set_direction(&ball, DIR_UP);
    }

    if (aabb_is_intersecting(&ball_box, &lpad_box)) {
        entity_set_direction(&ball, DIR_RIGHT);
    } else if (aabb_is_intersecting(&ball_box, &rpad_box)) {
        entity_set_direction(&ball, DIR_LEFT);
    }
}

static void apply_collision_to_paddles(void) {
    aabb_t lpad_box;
    aabb_t rpad_box;

    entity_get_aabb(&left_paddle, &lpad_box);
    entity_get_aabb(&right_paddle, &rpad_box);

    if (aabb_is_beyond_edge(&lpad_box, &field, AABB_TOP_EDGE)) {
        entity_set_direction(&left_paddle, DIR_DOWN);
    } else if (aabb_is_beyond_edge(&lpad_box, &field, AABB_BOTTOM_EDGE)) {
        entity_set_direction(&left_paddle, DIR_UP);
    }

    if (aabb_is_beyond_edge(&rpad_box, &field, AABB_TOP_EDGE)) {
        entity_set_direction(&right_paddle, DIR_DOWN);
    } else if (aabb_is_beyond_edge(&rpad_box, &field, AABB_BOTTOM_EDGE)) {
        entity_set_direction(&right_paddle, DIR_UP);
    }
}
/**
 * Primary game operations and timing loop.
 *
 * TODO: Separate main loop against game loop
 */
void do_update_process(float delta) {
    apply_collision_to_ball();
    apply_collision_to_paddles();
    physics_move_entities(3, (entity_t *[3]){&ball, &left_paddle, &right_paddle},
                          delta);
    check_goal_conditions();
}

/**
 * Processing block when STATE == PLAYING
 */
static void do_gameplay_loop(app_t *app, float delta) {
    do_inputs_process();
    do_update_process(delta);
    do_graphics_process(app);
}

// -------------------------------------
// Game FSM-Driver
// -------------------------------------

/**
 * Execute game processing blocks based on current game state.
 */
bool game_process_frame(app_t *app, float delta) {
    switch (game_state) {
    case INIT_STATE: // Start State
        do_initialize_game(app);
        break;
    case PLAYING_STATE:
        do_gameplay_loop(app, delta);
        break;
    case GAME_OVER_STATE:
        trigger(ALWAYS_TRIGGER);
        break;
    case TERM_STATE: // Stop State
        return false;
    // TODO:  Panic on unknown state!
    default:
        log_error("Reached unknown state (%d)", game_state);
        break;
    }
    return true;
}
