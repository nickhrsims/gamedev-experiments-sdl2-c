#include <stdint.h>
#include <stdlib.h>

#include "SDL2/SDL.h"
#include "log.h"

// -----------------------------------------------------------------------------
//
//     Data Definitions
//
// -----------------------------------------------------------------------------

// -------------------------------------
// Entities
// -------------------------------------

/**
 * Game Physics Body (Ball, Paddles, etc.)
 */
typedef struct {
    int16_t x, y;
    int16_t w, h;
    int16_t vx, vy;
} entity_t;

/**
 * Player Data
 */
typedef struct {
    uint64_t score;
} player_t;

// -------------------------------------
// Core System Structures
// -------------------------------------

/**
 * SDL2 Resource Container
 */
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} game_sdl_resource_container_t;

/**
 * Game Configuration Parameters
 */
typedef struct {
    char *window_title;
    uint16_t window_position_x;
    uint16_t window_position_y;
    uint16_t window_width;
    uint16_t window_height;
    uint8_t window_is_fullscreen;
} game_config_t;

/**
 * Game Instance Data
 */
typedef struct {
    game_sdl_resource_container_t sdl;
    game_config_t config;
    player_t player_1;
    player_t player_2;
    entity_t left_paddle;
    entity_t right_paddle;
    entity_t ball;

    uint8_t running;
} game_t;

// -----------------------------------------------------------------------------
//
//     Game Logic
//
// -----------------------------------------------------------------------------

// ---------------------------------------------------------
// Events
// ---------------------------------------------------------

void input_process(game_t *game) {
    uint8_t *is_game_running = &game->running;
    entity_t *lpad           = &game->left_paddle;
    entity_t *rpad           = &game->right_paddle;

    SDL_Event event;
    SDL_PollEvent(&event);

    uint8_t const *kb = SDL_GetKeyboardState(NULL);

    // Quit Game
    if (event.type == SDL_QUIT || kb[SDL_SCANCODE_ESCAPE]) {
        *is_game_running = 0;
    }

    // Left Paddle (A: UP, Z: DOWN)
    if (kb[SDL_SCANCODE_A]) {
        lpad->vy = -200;
    } else if (kb[SDL_SCANCODE_Z]) {
        lpad->vy = 200;
    } else {
        lpad->vy = 0;
    }

    // Right Paddle (K: UP, M: DOWN)
    if (kb[SDL_SCANCODE_K]) {
        rpad->vy = -200;
    } else if (kb[SDL_SCANCODE_M]) {
        rpad->vy = 200;
    } else {
        rpad->vy = 0;
    }

    return;
}

// ---------------------------------------------------------
// Graphics
// ---------------------------------------------------------

/**
 * Clear the screen.
 */
void graphics_clear(game_t *game) {
    SDL_RenderClear(game->sdl.renderer);
    return;
}

/**
 * Display all drawn entities.
 */
void graphics_show(game_t *game) {
    SDL_RenderPresent(game->sdl.renderer);
    return;
}

/**
 * Set entity draw color.
 */
void graphics_set_color(game_t *game, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(game->sdl.renderer, r, g, b, a);
    return;
}

/**
 * Reset entity draw color (set color to black).
 */
void graphics_reset_color(game_t *game) {
    graphics_set_color(game, 0, 0, 0, 0);
    return;
}

/**
 * Draw specific entity.
 */
void _graphics_draw_entity(game_t *game, entity_t *e) {
    SDL_RenderFillRect(game->sdl.renderer,
                       &(SDL_Rect){.x = e->x, .y = e->y, .w = e->w, .h = e->h});
    return;
}

/**
 * Draw all entities of given game instance.
 */
void graphics_draw_entities(game_t *game) {
    _graphics_draw_entity(game, &game->ball);
    _graphics_draw_entity(game, &game->left_paddle);
    _graphics_draw_entity(game, &game->right_paddle);
}

/**
 * Perform all graphics operations of a single frame.
 */
void graphics_process(game_t *game) {
    graphics_clear(game);
    graphics_set_color(game, 255, 255, 255, 255);
    graphics_draw_entities(game);
    graphics_reset_color(game);
    graphics_show(game);

    return;
}

// ---------------------------------------------------------
// Physics
// ---------------------------------------------------------

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
    uint16_t floor      = game->config.window_height;
    uint16_t ceiling    = 0;
    uint16_t right_wall = game->config.window_width;
    uint16_t left_wall  = 0;

    if (ball->x <= left_wall) {
        ball->vx = abs(ball->vx);
    }

    if (ball->y <= ceiling) {
        ball->vy = abs(ball->vy);
    }

    if (ball->x + ball->w >= right_wall) {
        ball->vx = -abs(ball->vx);
    }

    if (ball->y + ball->h >= floor) {
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

/**
 * Physics Processing Block
 */
void physics_process(game_t *game, float delta) {
    physics_move_entity(&game->ball, delta);
    physics_move_entity(&game->left_paddle, delta);
    physics_move_entity(&game->right_paddle, delta);
    physics_check_collision_with_edges(game);
    physics_check_collision_with_paddles(game);
    return;
}

// ---------------------------------------------------------
// Composition
// ---------------------------------------------------------

/**
 * Initialize a new game instance.
 *
 * Game instance is a singleton, subsequent calls will return the same
 * instance.
 */
game_t *game_init(game_config_t config) {

    // --- Singleton Control

    static game_t *game = NULL;
    // --- Return new instance
    if (game != NULL) {
        return game;
    }

    // --- Top-level initialization

    // SDL is initialized with the game instance.
    SDL_Init(SDL_INIT_VIDEO);

    game         = malloc(sizeof(game_t));
    game->config = config;

    // --- SDL2 Resources

    game->sdl.window = SDL_CreateWindow(
        config.window_title, config.window_position_x, config.window_position_y,
        config.window_width, config.window_height,
        config.window_is_fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

    // Game has no use for variable index or flags
    uint8_t const RENDERER_INDEX = 0;
    uint8_t const RENDERER_FLAGS = 0;

    game->sdl.renderer =
        SDL_CreateRenderer(game->sdl.window, RENDERER_INDEX, RENDERER_FLAGS);

    // --- Initialize Entity Properties

    { // Ball
        entity_t *b = &game->ball;
        // Place ball in center of screen.
        b->x  = game->config.window_width / 2;
        b->y  = game->config.window_height / 2;
        b->w  = 16;
        b->h  = 16;
        b->vx = 200;
        b->vy = 200;
    }

    { // Left Paddle
        entity_t *p = &game->left_paddle;
        p->x        = 128;
        p->y        = 64;
        p->w        = 8;
        p->h        = 128; // TODO: Relative size.
        p->vx       = 0;
        p->vy       = 0;
    }

    { // Right Paddle
        entity_t *p = &game->right_paddle;
        p->x        = (game->config.window_width) - 128;
        p->y        = 64;
        p->w        = 8;
        p->h        = 128; // TODO: Relative size.
        p->vx       = 0;
        p->vy       = 0;
    }

    { // Players
        player_t *p1 = &game->player_1;
        player_t *p2 = &game->player_2;

        p1->score = 0;
        p2->score = 0;
    }

    // --- Other Properties

    game->running = 0;

    // --- End

    return game;
}

/**
 * Primary game operations and timing loop.
 */
void game_loop(game_t *game) {

    /** CPU ticks at the start of the last frame. */
    uint64_t prev_frame_ticks = SDL_GetTicks64();

    /** CPU ticks at the start of this frame. */
    uint64_t curr_frame_ticks = 0;

    /** Time at the beginning of the frame. */
    uint64_t frame_start_time = 0;

    /** Time at the end of the frame. */
    uint64_t frame_end_time = 0;

    /** Milliseconds this frame has taken. */
    float elapsed_frame_ms = 0;

    /** Time between frames. Measured in seconds. */
    float delta = 0;

    game->running = 1;

    // --- Game Loop
    while (game->running == 1) {

        // --- Start Frame Timing

        frame_start_time = SDL_GetPerformanceCounter();
        curr_frame_ticks = SDL_GetTicks64();
        delta            = (curr_frame_ticks - prev_frame_ticks) / 1000.0f;

        input_process(game);
        physics_process(game, delta);
        graphics_process(game);

        // --- End Frame Timing
        //
        prev_frame_ticks = curr_frame_ticks;
        frame_end_time   = SDL_GetPerformanceCounter();
        elapsed_frame_ms = (frame_end_time - frame_start_time) /
                           (float)SDL_GetPerformanceFrequency() * 1000.0f;

        // 60 FPS in Milliseconds
        // == 1 (frame) / 60 (seconds) * 1000 (convert to ms)
        static float const FPS60 = 16.666f;

        // Delay each frame to get as close to 60FPS as possible.
        SDL_Delay(floor(FPS60 - elapsed_frame_ms));
    }

    return;
}

int main(void) {
    game_t *game = game_init((game_config_t){.window_is_fullscreen = 0,
                                             .window_width         = 640,
                                             .window_height        = 480,
                                             .window_position_x    = 128,
                                             .window_position_y    = 128,
                                             .window_title         = "Pong"});

    game_loop(game);

    SDL_Quit();
}
