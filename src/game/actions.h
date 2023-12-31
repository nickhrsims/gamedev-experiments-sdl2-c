#include <stdbool.h>

typedef enum {
  NULL_ACTION,
  MENU_UP,
  MENU_DOWN,
  MENU_LEFT,
  MENU_RIGHT,
  P1_UP,
  P1_DOWN,
  P2_UP,
  P2_DOWN,
  CONFIRM,
  CANCEL,
  PAUSE,
  QUIT,
  ACTION_COUNT,
} game_action_t;

void game_actions_refresh(void);
bool *game_actions_get(void);
