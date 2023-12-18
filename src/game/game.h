#pragma once

#include "app/app.h"

typedef struct game_s game_t;

game_t *game_init(app_config_t *config);
void game_term(game_t *game);
void game_run(game_t *game);
