#include <stdint.h>
#include <stdlib.h>

#include "SDL2/SDL.h"
#include "SDL_render.h"
#include "SDL_timer.h"
#include "SDL_video.h"
#include "log.h"

// -----------------------------------------------------------------------------
//
//     Data Definitions
//
// -----------------------------------------------------------------------------

// -------------------------------------
// Entities
// -------------------------------------

typedef struct {
    int pos_x, pos_y;
    int size_x, size_y;
    int vel_x, vel_y;
} entity_t;

typedef struct {
    int score;
} player_t;

// -------------------------------------
// Core System Structures
// -------------------------------------

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_resource_handle_container_t;

typedef struct {
    char *window_title;
    uint16_t window_pos_x;
    uint16_t window_pos_y;
    uint16_t window_size_x;
    uint16_t window_size_y;
    uint8_t is_window_fullscreen;
} game_config_t;

typedef struct {
    sdl_resource_handle_container_t resources;
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

void event_process(game_t *game) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
    case SDL_QUIT:
        game->running = 0;
        break;
    default:
        break;
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
    SDL_RenderClear(game->resources.renderer);
    return;
}

/**
 * Display all drawn entities.
 */
void graphics_show(game_t *game) {
    SDL_RenderPresent(game->resources.renderer);
    return;
}

/**
 * Set entity draw color.
 */
void graphics_set_color(game_t *game, uint8_t red, uint8_t green, uint8_t blue,
                        uint8_t alpha) {
    SDL_SetRenderDrawColor(game->resources.renderer, red, green, blue, alpha);
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
 * Draw ball.
 */
void _graphics_draw_entity(game_t *game, entity_t *e) {
    SDL_RenderFillRect(
        game->resources.renderer,
        &(SDL_Rect){.x = e->pos_x, .y = e->pos_y, .w = e->size_x, .h = e->size_y});
    return;
}

void graphics_draw_entities(game_t *game) {
    _graphics_draw_entity(game, &game->ball);
    _graphics_draw_entity(game, &game->left_paddle);
    _graphics_draw_entity(game, &game->right_paddle);
}

/**
 * Graphics Processing Block
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
void physics_move_ball(game_t *game, float delta) {
    entity_t *ball = &game->ball;
    ball->pos_x += ball->vel_x * delta;
    ball->pos_y += ball->vel_y * delta;
}

/**
 * Check collision between ball and edges.
 */
void physics_check_collision_with_edges(game_t *game) {
    entity_t *ball = &game->ball;

    // TODO: Use of config does not support resize.
    uint16_t floor      = game->config.window_size_y;
    uint16_t ceiling    = 0;
    uint16_t right_wall = game->config.window_size_x;
    uint16_t left_wall  = 0;

    if (ball->pos_x <= left_wall) {
        ball->vel_x = abs(ball->vel_x);
    }

    if (ball->pos_y <= ceiling) {
        ball->vel_y = abs(ball->vel_y);
    }

    if (ball->pos_x + ball->size_x >= right_wall) {
        ball->vel_x = -abs(ball->vel_x);
    }

    if (ball->pos_y + ball->size_y >= floor) {
        ball->vel_y = -abs(ball->vel_y);
    }
}

/**
 * Check collision between ball and paddles.
 * FIXME: Not implemented.
 */
void physics_check_collision_with_paddles(game_t *game) { return; }

/**
 * Physics Processing Block
 */
void physics_process(game_t *game, float delta) {
    physics_move_ball(game, delta);
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

    game->resources.window =
        SDL_CreateWindow(config.window_title, config.window_pos_x, config.window_pos_y,
                         config.window_size_x, config.window_size_y,
                         config.is_window_fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

    // Game has no use for variable index or flags
    int const RENDERER_INDEX = 0;
    int const RENDERER_FLAGS = 0;

    game->resources.renderer =
        SDL_CreateRenderer(game->resources.window, RENDERER_INDEX, RENDERER_FLAGS);

    // --- Initialize Entity Properties

    { // Ball
        entity_t *b = &game->ball;
        b->pos_x    = -128;
        b->pos_y    = -128;
        b->size_x   = 16;
        b->size_y   = 16;
        b->vel_x    = 200;
        b->vel_y    = 200;
    }

    { // Left Paddle
        entity_t *p = &game->left_paddle;
        p->pos_x    = 128;
        p->pos_y    = 64;
        p->size_x   = 8;
        p->size_y   = 128; // TODO: Relative size.
    }

    { // Right Paddle
        entity_t *p = &game->right_paddle;
        p->pos_x    = (game->config.window_size_x) - 128;
        p->pos_y    = 64;
        p->size_x   = 8;
        p->size_y   = 128; // TODO: Relative size.
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

void game_loop(game_t *game) {

    // Simulation Ticks for Delta Calculation
    uint64_t previous_frame_ticks = SDL_GetTicks64();
    uint64_t current_frame_ticks  = 0;

    // High-Resolution "Counts" for Frame Limiting
    uint64_t frame_start_hrc = 0;
    uint64_t frame_end_hrc   = 0;

    // Frame Time-Calculation Results
    float frame_elapsed_ms = 0;
    float delta            = 0;

    game->running = 1;

    // --- Game Loop
    while (game->running == 1) {

        // --- Start Frame Timing

        frame_start_hrc     = SDL_GetPerformanceCounter();
        current_frame_ticks = SDL_GetTicks64();
        delta               = (current_frame_ticks - previous_frame_ticks) / 1000.0f;

        event_process(game);
        physics_process(game, delta);
        graphics_process(game);

        // --- End Frame Timing
        //
        previous_frame_ticks = current_frame_ticks;
        frame_end_hrc        = SDL_GetPerformanceCounter();
        frame_elapsed_ms     = (frame_end_hrc - frame_start_hrc) /
                           (float)SDL_GetPerformanceFrequency() * 1000.0f;
        static float const FPS60 = 16.666f;
        SDL_Delay(floor(FPS60 - frame_elapsed_ms));
    }

    return;
}

int main(void) {
    game_t *game = game_init((game_config_t){.is_window_fullscreen = 0,
                                             .window_size_x        = 640,
                                             .window_size_y        = 480,
                                             .window_pos_x         = 128,
                                             .window_pos_y         = 128,
                                             .window_title         = "Pong"});

    game_loop(game);

    SDL_Quit();
}
