#include <stdbool.h>

#include "SDL.h"
#include "app.h"
#include "log.h"

// --- Initialization Flag
static bool is_app_initialized = false;
static bool is_app_running     = false;

// --- Window
#define DEFAULT_WINDOW_FLAGS 0

/**
 * Initialize static application.
 *
 * TODO: Return error code.
 */
void app_init(app_t *app, app_config_t *config) {

    log_set_level(LOG_DEBUG);

    // --- SDL2 System Initialization
    SDL_Init(SDL_INIT_VIDEO);

    // --- Window
    app->window = SDL_CreateWindow(
        config->window_title, config->window_position_x, config->window_position_y,
        config->window_width, config->window_height,
        config->window_is_fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

    // --- Font
    // TODO: Generalize font selection
    TTF_Init();
    app->font = TTF_OpenFont("res/font.ttf", 24);

    // --- Flags
    is_app_initialized = true;
    is_app_running     = true;
}

/**
 * Terminate static applicaiton.
 */
void app_term(app_t *app) {
    TTF_CloseFont(app->font);
    SDL_DestroyWindow(app->window);
    SDL_Quit();
}

void app_run(app_t *app, frame_processor_t process_frame) {
    /** Input Event Processing */
    SDL_Event event;

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

    // --- Application Loop
    while (is_app_running) {

        // --- Start Frame Timing

        frame_start_time = SDL_GetPerformanceCounter();
        curr_frame_ticks = SDL_GetTicks64();
        delta            = (curr_frame_ticks - prev_frame_ticks) / 1000.0f;

        // --- Poll input events
        SDL_PollEvent(&event);

        // --- Process Frame
        is_app_running = process_frame(app, delta);

        // --- Check OS-level quit request
        if (event.type == SDL_QUIT) {
            is_app_running = false;
        }

        // --- End Frame Timing
        //
        prev_frame_ticks = curr_frame_ticks;
        frame_end_time   = SDL_GetPerformanceCounter();
        elapsed_frame_ms = (frame_end_time - frame_start_time) /
                           (float)SDL_GetPerformanceFrequency() * 1000.0f;

        // 60 FPS in Milliseconds
        // == 1 (frame) / 60 (seconds) * 1000 (convert to ms)
        static float const FPS60 = 16.666f;

        long frame_delay = floor(FPS60 - elapsed_frame_ms);

        // Delay each frame to get as close to 60FPS as possible.
        if (frame_delay < 0)
            frame_delay = 0;
        SDL_Delay(frame_delay);
    }

    return;
}
