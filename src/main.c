#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "app.h"
#include "game/game.h"

static app_t app;

int main(void) {
    app_config_t config = {.window_is_fullscreen = 0,
                           .window_width         = 640,
                           .window_height        = 480,
                           .window_position_x    = 128,
                           .window_position_y    = 128,
                           .window_title         = "Pong"};

    app_init(&app, &config);
    game_init(&app);
    app_run(&app, game_process_frame);
    app_term(&app);
}
