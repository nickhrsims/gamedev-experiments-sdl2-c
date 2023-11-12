#include <SDL2/SDL.h>

#include "game.h"

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
