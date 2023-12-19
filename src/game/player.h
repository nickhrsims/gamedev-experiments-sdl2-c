#pragma once

/**
 * Player Data
 */
typedef struct {
  unsigned short score;
} player_t;

/**
 * Get score of a given player.
 */
unsigned short player_get_score(player_t *player);

/**
 * Increment the score of a given player by 1.
 */
void player_inc_score(player_t *player);
