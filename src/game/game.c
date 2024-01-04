#include <SDL2/SDL.h>

#include <log.h>
#include <stddef.h>

#include "SDL_events.h"
#include "SDL_scancode.h"

#include "app/app.h"
#include "app/video.h"

#include "aabb.h"
#include "actions.h"
#include "alloc.h"
#include "ball.h"
#include "collision.h"
#include "entity.h"
#include "field.h"
#include "fsm/fsm.h"
#include "game.h"
#include "paddle.h"
#include "player.h"

// -----------------------------------------------------------------------------
// Core Data Types
// -----------------------------------------------------------------------------

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

static size_t const entity_count = 3;
static entity_t *entity_pool[3]  = {&ball, &left_paddle, &right_paddle};

// Input Configuration
static action_table_cfg_t action_table_config = {
    [MENU_UP] = SDL_SCANCODE_UP,     [MENU_DOWN] = SDL_SCANCODE_DOWN,
    [MENU_LEFT] = SDL_SCANCODE_LEFT, [MENU_RIGHT] = SDL_SCANCODE_RIGHT,
    [P1_UP] = SDL_SCANCODE_A,        [P1_DOWN] = SDL_SCANCODE_Z,
    [P2_UP] = SDL_SCANCODE_K,        [P2_DOWN] = SDL_SCANCODE_M,
    [CONFIRM] = SDL_SCANCODE_RETURN, [CANCEL] = SDL_SCANCODE_BACKSPACE,
    [PAUSE] = SDL_SCANCODE_P,        [QUIT] = SDL_SCANCODE_ESCAPE,
};

// Action Table (Input Map Instance)
static action_table_t *action_table;

// State Machine Handle
fsm_t *fsm;

// State Options
enum game_state_enum {
    STATE_GUARD,
    START_STATE,
    PLAYING_STATE,
    PAUSE_STATE,
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
    PAUSE_TRIGGER,
    TRIGGER_COUNT,
    CONFIRM_TRIGGER,
    CANCEL_TRIGGER,
};

/**
 * Configure game-state transition table.
 */
static void configure_fsm(void) {
    fsm = fsm_init(STATE_COUNT, TRIGGER_COUNT, START_STATE);

    // Start
    fsm_on(fsm, START_STATE, ALWAYS_TRIGGER, PLAYING_STATE);
    fsm_on(fsm, START_STATE, QUIT_GAME_TRIGGER, TERM_STATE);

    // Playing
    fsm_on(fsm, PLAYING_STATE, GAME_OVER_TRIGGER, GAME_OVER_STATE);
    fsm_on(fsm, PLAYING_STATE, QUIT_GAME_TRIGGER, TERM_STATE);
    fsm_on(fsm, PLAYING_STATE, PAUSE_TRIGGER, PAUSE_STATE);

    // Pause State
    fsm_on(fsm, PAUSE_STATE, QUIT_GAME_TRIGGER, TERM_STATE);
    fsm_on(fsm, PAUSE_STATE, PAUSE_TRIGGER, PLAYING_STATE);

    // Game Over
    fsm_on(fsm, GAME_OVER_STATE, CONFIRM_TRIGGER, TERM_STATE);
    fsm_on(fsm, GAME_OVER_STATE, CANCEL_TRIGGER, TERM_STATE);
    fsm_on(fsm, GAME_OVER_STATE, QUIT_GAME_TRIGGER, TERM_STATE);

    // Terminating
    fsm_on(fsm, TERM_STATE, ALWAYS_TRIGGER, TERM_STATE);
}

/**
 * Draw all entities of given game instance.
 */
static void draw_entities(video_t *video, size_t entity_count,
                          entity_t *entity_pool[entity_count]) {
    for (size_t entity_num = 0; entity_num < entity_count; entity_num++) {
        entity_t *e = entity_pool[entity_num];
        video_draw_region(video, &e->transform);
    }
}

static void check_goal_conditions(void) {

    static unsigned char const winning_score = 5;

    // Is the ball in the left goal?
    if (field_is_subject_in_left_goal(&field, &ball.transform)) {
        // player 2 gets the point
        player_inc_score(&player_2);
        ball_configure(&ball, &field);
    }

    // Is the ball in the right goal?
    else if (field_is_subject_in_right_goal(&field, &ball.transform)) {
        // player 1 gets the point
        player_inc_score(&player_1);
        ball_configure(&ball, &field);
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

// -----------------------------------------------------------------------------
// Core Processing Blocks
// -----------------------------------------------------------------------------

/**
 * Processing block when STATE == PLAYING
 */
static void do_playing_state(app_t *app, float delta) {
    // --- Input
    bool *actions = action_table_get_binary_states(action_table);

    bool p1_up   = actions[P1_UP];
    bool p1_down = actions[P1_DOWN];
    bool p2_up   = actions[P2_UP];
    bool p2_down = actions[P2_DOWN];

    entity_set_velocity(&left_paddle, 0, (p1_down - p1_up) * 200);
    entity_set_velocity(&right_paddle, 0, (p2_down - p2_up) * 200);

    // --- Update

    // Collision
    collision_process(entity_count, entity_pool);
    collision_out_of_bounds_process(entity_count, entity_pool, &field);

    // Entity Updates
    for (size_t entity_index = 0; entity_index < entity_count; entity_index++) {
        entity_t *e = entity_pool[entity_index];
        e->update(e, delta);
    }

    // Goal Polling
    check_goal_conditions();

    // --- Output
    static uint8_t SCORE_STRING_LENGTH = 5; // 5 digits + sentinel
    char p1_score_str[SCORE_STRING_LENGTH + 1];
    char p2_score_str[SCORE_STRING_LENGTH + 1];

    // TODO: Abstract into itoa-like func for players
    snprintf(p1_score_str, SCORE_STRING_LENGTH, "%hu", player_get_score(&player_1));
    snprintf(p2_score_str, SCORE_STRING_LENGTH, "%hu", player_get_score(&player_2));

    video_reset_color(app->video);
    video_clear(app->video);
    video_set_color(app->video, 255, 255, 255, 255);
    draw_entities(app->video, 3, (entity_t *[3]){&ball, &left_paddle, &right_paddle});
    video_draw_text(app->video, p1_score_str, (field.x + field.w) / 2 - 48, 16);
    video_draw_text(app->video, p2_score_str, (field.x + field.w) / 2 + 48, 16);
    video_render(app->video);
}

void do_start_state(app_t *app, float delta) {

    // --- Game Action Inputs
    bool *actions = action_table_get_binary_states(action_table);

    if (actions[CONFIRM]) {
        fsm_trigger(fsm, ALWAYS_TRIGGER);
    } else if (actions[QUIT]) {
        fsm_trigger(fsm, QUIT_GAME_TRIGGER);
    }

    // --- State Actors
    static unsigned short const speed = 301;
    static unsigned char alpha        = 100;
    static float alpha_direction      = speed;

    // --- Animation Update
    // Bounce Effect
    if (alpha <= 60) {
        alpha_direction = speed;
    } else if (alpha >= 236) {
        alpha_direction = -speed;
    }
    // Animation Driver
    alpha += alpha_direction * delta;

    // --- Rendering
    video_reset_color(app->video);
    video_clear(app->video);
    video_draw_text_with_color(app->video, "Press Enter", field.x + (field.w / 2),
                               field.y + (field.h / 2), 255, 255, 255, alpha);
    video_render(app->video);
}

static void do_pause_state(app_t *app, float delta) {

    // --- State Actors
    static unsigned short const speed = 301;
    static unsigned char alpha        = 100;
    static float alpha_direction      = speed;

    // --- Animation Update
    // Bounce Effect
    if (alpha <= 60) {
        alpha_direction = speed;
    } else if (alpha >= 236) {
        alpha_direction = -speed;
    }
    // Animation Driver
    alpha += alpha_direction * delta;

    // --- Extra Rendering
    static uint8_t SCORE_STRING_LENGTH = 5; // 5 digits + sentinel
    char p1_score_str[SCORE_STRING_LENGTH + 1];
    char p2_score_str[SCORE_STRING_LENGTH + 1];

    // TODO: Abstract into itoa-like func for players
    snprintf(p1_score_str, SCORE_STRING_LENGTH, "%hu", player_get_score(&player_1));
    snprintf(p2_score_str, SCORE_STRING_LENGTH, "%hu", player_get_score(&player_2));

    // --- Rendering
    // Clear Renderer
    video_reset_color(app->video);
    video_clear(app->video);
    // Draw Faded Entities
    video_set_color(app->video, 90, 90, 90, 90);
    draw_entities(app->video, 3, (entity_t *[3]){&ball, &left_paddle, &right_paddle});
    // Fraw Faded Scores
    video_draw_text_with_color(app->video, p1_score_str, (field.x + field.w) / 2 - 48,
                               16, 255, 255, 255, 90);
    video_draw_text_with_color(app->video, p2_score_str, (field.x + field.w) / 2 + 48,
                               16, 255, 255, 255, 90);
    // Draw Flashing Pause Text
    video_draw_text_with_color(app->video, "Paused", field.x + (field.w / 2),
                               field.y + (field.h / 2), 255, 255, 255, alpha);
    // Finalize
    video_render(app->video);
}

void do_game_over_state(app_t *app, float delta) {

    // --- Game Action Inputs
    bool *actions = action_table_get_binary_states(action_table);

    if (actions[CONFIRM]) {
        fsm_trigger(fsm, ALWAYS_TRIGGER);
    } else if (actions[QUIT] || actions[CANCEL]) {
        fsm_trigger(fsm, QUIT_GAME_TRIGGER);
    }

    // --- State Actors
    static unsigned short const speed = 301;
    static unsigned char alpha        = 100;
    static float alpha_direction      = speed;

    // --- Animation Update
    // Bounce Effect
    if (alpha <= 60) {
        alpha_direction = speed;
    } else if (alpha >= 236) {
        alpha_direction = -speed;
    }
    // Animation Driver
    alpha += alpha_direction * delta;

    // --- Rendering
    video_reset_color(app->video);
    video_clear(app->video);
    video_draw_text_with_color(app->video, "Game Over", field.x + (field.w / 2),
                               field.y + (field.h / 2), 255, 255, 255, alpha);
    video_render(app->video);
}

// -----------------------------------------------------------------------------
// Game-App Infrastructure
// -----------------------------------------------------------------------------

/**
 * Handle incoming game events one at a time.
 */
static void handle_event(app_t *app, SDL_Event *event) {
    (void)app;

    if (event->type == SDL_KEYDOWN) {
        SDL_Scancode scancode = event->key.keysym.scancode;
        action_t action = action_table_get_scancode_action(action_table, scancode);
        switch (action) {
        case PAUSE:
            fsm_trigger(fsm, PAUSE_TRIGGER);
            break;
        case QUIT:
            fsm_trigger(fsm, QUIT_GAME_TRIGGER);
            break;
        default:
            break;
        }
    }

    return;
}

/**
 * Execute game processing blocks based on current game state.
 */
static void handle_frame(app_t *app, float delta) {

    switch (fsm_state(fsm)) {
    case START_STATE: // Start State
        do_start_state(app, delta);
        break;
    case PLAYING_STATE:
        do_playing_state(app, delta);
        break;
    case PAUSE_STATE:
        do_pause_state(app, delta);
        break;
    case GAME_OVER_STATE:
        do_game_over_state(app, delta);
        break;
    case TERM_STATE: // Stop State
        app_stop(app);
        break;
    // TODO:  Panic on unknown state!
    default:
        log_error("Reached unknown state (%d)", fsm_state(fsm));
        break;
    }
}

/**
 * Begin processing of the main game loop.
 */
void game_run(game_t *game) { app_run(game->app, handle_frame, handle_event); }

/**
 * Initialize game instance.
 */
game_t *game_init(app_config_t *config) {
    log_debug("Initializing Game");

    // --- Application Initializer
    game_t *game = new (game_t);
    game->app    = NULL;

    if (!(game->app = app_init(config))) {
        game_term(game);
        return NULL;
    }

    // --- Field Configuration
    int window_width, window_height;
    video_get_window_size(game->app->video, &window_width, &window_height);
    field.w = window_width;
    field.h = window_height;

    // --- Entity Configuration
    ball_configure(&ball, &field);
    paddle_configure(&left_paddle, &field, LEFT_PADDLE);
    paddle_configure(&right_paddle, &field, RIGHT_PADDLE);

    // --- Action Table
    action_table = action_table_init(action_table_config);

    // --- FSM
    configure_fsm();

    log_debug("Initialization Complete");
    return game;
}

/**
 * Terminate game instance.
 */
void game_term(game_t *game) {
    if (!game) {
        return;
    }
    fsm_term(fsm);
    action_table_term(action_table);
    app_term(game->app);

    delete (game);
}
