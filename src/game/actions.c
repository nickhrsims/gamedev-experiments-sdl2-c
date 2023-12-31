#include "SDL_keyboard.h"
#include "SDL_scancode.h"

#include "actions.h"

static SDL_Scancode game_action_map[ACTION_COUNT] = {
    [MENU_UP] = SDL_SCANCODE_UP,     [MENU_DOWN] = SDL_SCANCODE_DOWN,
    [MENU_LEFT] = SDL_SCANCODE_LEFT, [MENU_RIGHT] = SDL_SCANCODE_RIGHT,
    [P1_UP] = SDL_SCANCODE_A,        [P1_DOWN] = SDL_SCANCODE_Z,
    [P2_UP] = SDL_SCANCODE_K,        [P2_DOWN] = SDL_SCANCODE_M,
    [CONFIRM] = SDL_SCANCODE_RETURN, [CANCEL] = SDL_SCANCODE_BACKSPACE,
    [PAUSE] = SDL_SCANCODE_P,        [QUIT] = SDL_SCANCODE_ESCAPE,
};

static bool game_actions[ACTION_COUNT] = {0};

void game_actions_refresh(void) {
    // Polling is done in app layer
    uint8_t const *kb = SDL_GetKeyboardState(NULL);

    for (size_t action_index = 0; action_index < ACTION_COUNT; action_index++) {
        game_actions[action_index] = kb[game_action_map[action_index]];
    }
}

bool *game_actions_get(void) { return game_actions; }
