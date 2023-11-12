#pragma once

#include "game.h"

void physics_move_entity(entity_t *, float);
void physics_check_collision_with_edges(game_t *);
void physics_check_collision_with_paddles(game_t *);
