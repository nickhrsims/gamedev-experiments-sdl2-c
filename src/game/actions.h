#include <stdbool.h>

#include "SDL.h"
#include "SDL_scancode.h"

// Actions are keyboard only, and only support SDL Scancodes!

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
} action_t;

typedef struct action_table_s action_table_t;

typedef SDL_Scancode action_table_cfg_t[ACTION_COUNT];

action_table_t *action_table_init(action_table_cfg_t config);

void action_table_term(action_table_t *table);

action_t action_table_get_scancode_action(action_table_t *table,
                                          SDL_Scancode scancode);

bool *action_table_get_binary_states(action_table_t *table);
