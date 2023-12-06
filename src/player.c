#include "player.h"

/**
 * Get score of a given player.
 */
unsigned short player_get_score(player_t *player) { return player->score; }

/**
 * Increment the score of a given player by 1.
 */
void player_inc_score(player_t *player) { player->score++; }
