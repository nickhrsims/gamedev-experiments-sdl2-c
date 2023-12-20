#include <SDL2/SDL.h>

#include <log.h>

#include "SDL_scancode.h"

#include "aabb.h"
#include "alloc.h"
#include "app/app.h"
#include "app/video.h"
#include "ball.h"
#include "entity.h"
#include "field.h"
#include "fsm/fsm.h"
#include "game.h"
#include "paddle.h"
#include "player.h"

// -----------------------------------------------------------------------------
// Core Data Types
// -----------------------------------------------------------------------------

// NOTE: Current entity pool should be immutable.
typedef struct entity_pool_s {
    size_t entity_count;
    entity_t *entities;
} entity_pool_t;

/**
 * Game. Sub type of App.
 */
typedef struct game_s {
    app_t *app;
} game_t;

// -----------------------------------------------------------------------------
// Game Components
// -----------------------------------------------------------------------------
static player_t player_1     = {0}; // Players do not need configuration.
static player_t player_2     = {0}; // Just init to zero and away we go!
static entity_t ball         = {0};
static entity_t left_paddle  = {0};
static entity_t right_paddle = {0};
static aabb_t field          = {0};

// -----------------------------------------------------------------------------
// State Machine
// -----------------------------------------------------------------------------

// State Machine Handle
fsm_t *fsm;

// State Options
enum game_state_enum {
    STATE_GUARD,
    START_STATE,
    PLAYING_STATE,
    GAME_OVER_STATE,
    TERM_STATE,
    STATE_COUNT,
};

// Trigger Options
enum game_trigger_enum {
    INIT_DONE_TRIGGER,
    QUIT_GAME_TRIGGER,
    GAME_OVER_TRIGGER,
    ALWAYS_TRIGGER,
    TRIGGER_COUNT,
};

// -----------------------------------------------------------------------------
// Game Actions * Input Processing
// -----------------------------------------------------------------------------

typedef enum {
    NULL_ACTION,
    MENU_UP,
    MENU_DOWN,
    MENU_LEFT,
    MENU_RIGHT,
    P1_UP,
    P1_DOWN,
    P2_UP,
    P2_DOWN,
    CONFIRM,
    CANCEL,
    QUIT,
    ACTION_COUNT,
} game_action_t;

static SDL_Scancode game_action_map[ACTION_COUNT] = {
    [MENU_UP] = SDL_SCANCODE_UP,     [MENU_DOWN] = SDL_SCANCODE_DOWN,
    [MENU_LEFT] = SDL_SCANCODE_LEFT, [MENU_RIGHT] = SDL_SCANCODE_RIGHT,
    [P1_UP] = SDL_SCANCODE_A,        [P1_DOWN] = SDL_SCANCODE_Z,
    [P2_UP] = SDL_SCANCODE_K,        [P2_DOWN] = SDL_SCANCODE_M,
    [CONFIRM] = SDL_SCANCODE_RETURN, [CANCEL] = SDL_SCANCODE_BACKSPACE,
    [QUIT] = SDL_SCANCODE_ESCAPE,
};

static bool game_actions[ACTION_COUNT] = {0};

static void collect_actions_from_keyboard(void) {
    // Polling is done in app layer
    uint8_t const *kb = SDL_GetKeyboardState(NULL);

    for (size_t action_index = 0; action_index < ACTION_COUNT; action_index++) {
        game_actions[action_index] = kb[game_action_map[action_index]];
    }
}

static void process_actions(void) {
    if (game_actions[QUIT]) {
        fsm_trigger(fsm, QUIT_GAME_TRIGGER);
    }

    bool p1_up   = game_actions[P1_UP];
    bool p1_down = game_actions[P1_DOWN];
    bool p2_up   = game_actions[P2_UP];
    bool p2_down = game_actions[P2_DOWN];

    entity_set_velocity(&left_paddle, 0, (p1_down - p1_up) * 200);
    entity_set_velocity(&right_paddle, 0, (p2_down - p2_up) * 200);

    return;
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
 * Draw all entities of given game instance.
 * TODO: Fix via. first-party AABB in app layer.
 */
static void draw_entities(video_t *video, size_t entity_count,
                          entity_t *entity_pool[entity_count]) {
    for (size_t entity_num = 0; entity_num < entity_count; entity_num++) {
        entity_t *e = entity_pool[entity_num];
        video_draw_region(video, e->x, e->y, e->w, e->h);
    }
}

/**
 * Graphics processing block.
 */
static void do_graphics_process(app_t *app) {
    static uint8_t SCORE_STRING_LENGTH = 5; // 5 digits + sentinel
    char p1_score_str[SCORE_STRING_LENGTH + 1];
    char p2_score_str[SCORE_STRING_LENGTH + 1];

    // TODO: Abstract into itoa-like func for players
    snprintf(p1_score_str, SCORE_STRING_LENGTH, "%hu", player_get_score(&player_1));
    snprintf(p2_score_str, SCORE_STRING_LENGTH, "%hu", player_get_score(&player_2));

    video_clear(app->video);
    video_set_color(app->video, 255, 255, 255, 255);
    draw_entities(app->video, 3, (entity_t *[3]){&ball, &left_paddle, &right_paddle});
    video_draw_text(app->video, p1_score_str, (field.x + field.w) / 2 - 48, 16);
    video_draw_text(app->video, p2_score_str, (field.x + field.w) / 2 + 48, 16);
    video_reset_color(app->video);
    video_render(app->video);

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
        fsm_trigger(fsm, GAME_OVER_TRIGGER);
    }

    // Did player 2 win?
    else if (player_get_score(&player_2) >= winning_score) {
        fsm_trigger(fsm, GAME_OVER_TRIGGER);
    }
}

// -------------------------------------
// Per-State Processing Blocks
// -------------------------------------

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

    switch (aabb_get_intersection(&ball_box, &lpad_box)) {
    case AABB_LEFT_EDGE:
        log_debug("LEFT");
        entity_set_direction(&ball, DIR_RIGHT);
        break;
    case AABB_RIGHT_EDGE:
        log_debug("RIGHT");
        entity_set_direction(&ball, DIR_LEFT);
        break;
    case AABB_TOP_EDGE:
        log_debug("TOP");
        entity_set_direction(&ball, DIR_DOWN);
        break;
    case AABB_BOTTOM_EDGE:
        log_debug("BOTTOM");
        entity_set_direction(&ball, DIR_UP);
        break;
    case AABB_NO_EDGE:
        break;
    default:
        break;
    }

    switch (aabb_get_intersection(&ball_box, &rpad_box)) {
    case AABB_LEFT_EDGE:
        log_debug("LEFT");
        entity_set_direction(&ball, DIR_RIGHT);
        break;
    case AABB_RIGHT_EDGE:
        log_debug("RIGHT");
        entity_set_direction(&ball, DIR_LEFT);
        break;
    case AABB_TOP_EDGE:
        log_debug("TOP");
        entity_set_direction(&ball, DIR_DOWN);
        break;
    case AABB_BOTTOM_EDGE:
        log_debug("BOTTOM");
        entity_set_direction(&ball, DIR_UP);
        break;
    case AABB_NO_EDGE:
        break;
    default:
        break;
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

static void update_entities(size_t entity_count, entity_t *entity_pool[entity_count],
                            float delta) {
    for (size_t entity_index = 0; entity_index < entity_count; entity_index++) {
        entity_t *e = entity_pool[entity_index];
        e->update(e, delta);
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
    update_entities(3, (entity_t *[3]){&ball, &left_paddle, &right_paddle}, delta);
    check_goal_conditions();
}

/**
 * Processing block when STATE == PLAYING
 */
static void do_gameplay_loop(app_t *app, float delta) {
    collect_actions_from_keyboard();
    process_actions();
    do_update_process(delta);
    do_graphics_process(app);
}

// -------------------------------------
// Game FSM-Driver
// -------------------------------------

static void initialize_game_fsm(void) {
    fsm = fsm_init(STATE_COUNT, TRIGGER_COUNT, START_STATE);

    // Start
    fsm_on(fsm, START_STATE, ALWAYS_TRIGGER, PLAYING_STATE);

    // Playing
    fsm_on(fsm, PLAYING_STATE, GAME_OVER_TRIGGER, GAME_OVER_STATE);
    fsm_on(fsm, PLAYING_STATE, QUIT_GAME_TRIGGER, TERM_STATE);

    // Game Over
    fsm_on(fsm, GAME_OVER_STATE, ALWAYS_TRIGGER, TERM_STATE);

    // Terminating
    fsm_on(fsm, TERM_STATE, ALWAYS_TRIGGER, TERM_STATE);
}

static void terminate_game_fsm(fsm_t *fsm) { fsm_term(fsm); }

game_t *game_init(app_config_t *config) {
    log_debug("Initializing Game");

    // --- Application Initializer
    game_t *game = new (game_t);
    game->app    = app_init(config);

    // --- Field Configuration
    int window_width, window_height;
    video_get_window_size(game->app->video, &window_width, &window_height);
    field.w = window_width;
    field.h = window_height;

    // --- Entity Configuration
    ball_configure(&ball, &field);
    paddle_configure(&left_paddle, &field, LEFT_PADDLE);
    paddle_configure(&right_paddle, &field, RIGHT_PADDLE);

    initialize_game_fsm();

    log_debug("Initialization Complete");
    return game;
}

// NOTE: UNUSED
void game_term(game_t *game) {
    terminate_game_fsm(fsm);
    app_term(game->app);
}

static void game_process_event(app_t *app, event_t *event) {
    (void)app;
    (void)event;
    return;
}

/**
 * Execute game processing blocks based on current game state.
 */
static bool game_process_frame(app_t *app, float delta) {
    switch (fsm_state(fsm)) {
    case START_STATE: // Start State
        fsm_trigger(fsm, ALWAYS_TRIGGER);
        break;
    case PLAYING_STATE:
        do_gameplay_loop(app, delta);
        break;
    case GAME_OVER_STATE:
        fsm_trigger(fsm, ALWAYS_TRIGGER);
        break;
    case TERM_STATE: // Stop State
        return false;
    // TODO:  Panic on unknown state!
    default:
        log_error("Reached unknown state (%d)", fsm_state(fsm));
        break;
    }
    return true;
}

void game_run(game_t *game) {
    app_run(game->app, game_process_frame, game_process_event);
}
