#include <string.h>

#include "SDL_keyboard.h"
#include "SDL_scancode.h"

#include "actions.h"
#include "alloc.h"

typedef action_t scancode_to_action_map_t[SDL_NUM_SCANCODES];
typedef bool binary_action_state_table_t[ACTION_COUNT];

typedef struct action_table_s {
    action_table_cfg_t action_to_scancode_map;
    scancode_to_action_map_t scancode_to_action_map;
    binary_action_state_table_t binary_action_state_table;
} action_table_t;

action_table_t *action_table_init(action_table_cfg_t config) {
    action_table_t *table = new (action_table_t);

    // --- Initialize Primary Table
    memcpy(table->action_to_scancode_map, config, sizeof(action_table_cfg_t));

    // --- Initialialize Secondary Lookup Table
    for (action_t action = 0; action < ACTION_COUNT; action++) {
        table->scancode_to_action_map[table->action_to_scancode_map[action]] = action;
    }

    return table;
}

void action_table_term(action_table_t *table) {
    if (!table) {
        return;
    }
    delete (table);
}

action_t action_table_get_scancode_action(action_table_t *table,
                                          SDL_Scancode scancode) {
    return table->scancode_to_action_map[scancode];
}

bool *action_table_get_binary_states(action_table_t *table) {
    // Polling is done in app layer.
    uint8_t const *kb = SDL_GetKeyboardState(NULL);

    for (size_t action = 0; action < ACTION_COUNT; action++) {
        table->binary_action_state_table[action] =
            kb[table->action_to_scancode_map[action]];
    }

    return table->binary_action_state_table;
}
