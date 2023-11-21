#include <stdbool.h>

#include <SDL2/SDL.h>

#include "game.h"
#include "graphics.h"
#include "physics.h"

/**
 * Input processing block.
 */
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

/**
 * Graphics processing block.
 */
void graphics_process(game_t *game) {
    graphics_clear(game);
    graphics_set_color(game, 255, 255, 255, 255);
    graphics_draw_entities(game);
    graphics_reset_color(game);
    graphics_show(game);

    return;
}

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
    TTF_Init();

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

    // TODO: Generalize font selection
    game->sdl.font = TTF_OpenFont("res/font.ttf", 24);

    // --- Initialize Entity Properties

    // TODO: Abstract out

    { // Ball
        entity_t *b = &game->ball;
        // Place ball in center of screen.
        b->x  = game->config.window_width / 2;
        b->y  = game->config.window_height / 2;
        b->w  = 16;
        b->h  = 16;
        b->vx = -200;
        b->vy = -200;
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


void rules_apply(game_t *game) {}

/**
 * Primary game operations and timing loop.
 *
 * TODO: Separate main loop against game loop
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
        physics_apply(game, delta);
        rules_apply(game);
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
