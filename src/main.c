#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "app/app.h"
#include "game/game.h"

int main(void) {
    app_config_t config = {.window_is_fullscreen = 0,
                           .window_width         = 640,
                           .window_height        = 480,
                           .window_position_x    = 128,
                           .window_position_y    = 128,
                           .window_title         = "Pong"};

    game_t *game = game_init(&config);
    game_run(game);
    game_term(game);
}
