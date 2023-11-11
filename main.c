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
    SDL_Window *win;
    SDL_Renderer *ren;
} game_sdl_res_t;

/**
 * Game Configuration Parameters
 */
typedef struct {
    char *win_title;
    uint16_t win_x;
    uint16_t win_y;
    uint16_t win_w;
    uint16_t win_h;
    uint8_t win_fullscrn;
} game_config_t;

/**
 * Game Instance Data
 */
typedef struct {
    game_sdl_res_t sdl;
    game_config_t cfg;
    player_t p1;
    player_t p2;
    entity_t lpad;
    entity_t rpad;
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
    SDL_Event event;
    SDL_PollEvent(&event);

    uint8_t const *kb = SDL_GetKeyboardState(NULL);

    // Quit Game
    if (event.type == SDL_QUIT || kb[SDL_SCANCODE_ESCAPE]) {
        game->running = 0;
    }

    // Left Paddle (A: UP, Z: DOWN)
    if (kb[SDL_SCANCODE_A]) {
        game->lpad.vy = -200;
    } else if (kb[SDL_SCANCODE_Z]) {
        game->lpad.vy = 200;
    } else {
        game->lpad.vy = 0;
    }

    // Right Paddle (K: UP, M: DOWN)
    if (kb[SDL_SCANCODE_K]) {
        game->rpad.vy = -200;
    } else if (kb[SDL_SCANCODE_M]) {
        game->rpad.vy = 200;
    } else {
        game->rpad.vy = 0;
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
    SDL_RenderClear(game->sdl.ren);
    return;
}

/**
 * Display all drawn entities.
 */
void graphics_show(game_t *game) {
    SDL_RenderPresent(game->sdl.ren);
    return;
}

/**
 * Set entity draw color.
 */
void graphics_set_color(game_t *game, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(game->sdl.ren, r, g, b, a);
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
    SDL_RenderFillRect(game->sdl.ren,
                       &(SDL_Rect){.x = e->x, .y = e->y, .w = e->w, .h = e->h});
    return;
}

/**
 * Draw all entities of given game instance.
 */
void graphics_draw_entities(game_t *game) {
    _graphics_draw_entity(game, &game->ball);
    _graphics_draw_entity(game, &game->lpad);
    _graphics_draw_entity(game, &game->rpad);
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
    uint16_t floor      = game->cfg.win_h;
    uint16_t ceiling    = 0;
    uint16_t right_wall = game->cfg.win_w;
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

/**
 * Check collision between ball and paddles.
 * FIXME: Not implemented.
 */
void physics_check_collision_with_paddles(game_t *game) { return; }

/**
 * Physics Processing Block
 */
void physics_process(game_t *game, float delta) {
    physics_move_entity(&game->ball, delta);
    physics_move_entity(&game->lpad, delta);
    physics_move_entity(&game->rpad, delta);
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

    game      = malloc(sizeof(game_t));
    game->cfg = config;

    // --- SDL2 Resources

    game->sdl.win =
        SDL_CreateWindow(config.win_title, config.win_x, config.win_y, config.win_w,
                         config.win_h, config.win_fullscrn ? SDL_WINDOW_FULLSCREEN : 0);

    // Game has no use for variable index or flags
    uint8_t const RENDERER_INDEX = 0;
    uint8_t const RENDERER_FLAGS = 0;

    game->sdl.ren = SDL_CreateRenderer(game->sdl.win, RENDERER_INDEX, RENDERER_FLAGS);

    // --- Initialize Entity Properties

    { // Ball
        entity_t *b = &game->ball;
        // Place ball in center of screen.
        b->x  = game->cfg.win_w / 2;
        b->y  = game->cfg.win_h / 2;
        b->w  = 16;
        b->h  = 16;
        b->vx = 200;
        b->vy = 200;
    }

    { // Left Paddle
        entity_t *p = &game->lpad;
        p->x        = 128;
        p->y        = 64;
        p->w        = 8;
        p->h        = 128; // TODO: Relative size.
        p->vx       = 0;
        p->vy       = 0;
    }

    { // Right Paddle
        entity_t *p = &game->rpad;
        p->x        = (game->cfg.win_w) - 128;
        p->y        = 64;
        p->w        = 8;
        p->h        = 128; // TODO: Relative size.
        p->vx       = 0;
        p->vy       = 0;
    }

    { // Players
        player_t *p1 = &game->p1;
        player_t *p2 = &game->p2;

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
    game_t *game = game_init((game_config_t){.win_fullscrn = 0,
                                             .win_w        = 640,
                                             .win_h        = 480,
                                             .win_x        = 128,
                                             .win_y        = 128,
                                             .win_title    = "Pong"});

    game_loop(game);

    SDL_Quit();
}
