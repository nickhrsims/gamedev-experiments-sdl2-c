#include <assert.h>
#include <stdlib.h>

#include "fsm.h"

fsm_state_id_t fsm_get_target(size_t state_count, size_t trigger_count,
                              fsm_state_id_t fsm[state_count][trigger_count],
                              fsm_trigger_id_t trigger, fsm_state_id_t current_state) {
    assert(fsm);
    assert(fsm[current_state]);
    return fsm[current_state][trigger];
}

void fsm_send(size_t state_count, size_t trigger_count,
              fsm_state_id_t fsm[state_count][trigger_count], fsm_trigger_id_t trigger,
              fsm_state_id_t *current_state) {
    assert(fsm);
    assert(fsm[current_state]);
    fsm_state_id_t next_state =
        fsm_get_target(state_count, trigger_count, fsm, trigger, *current_state);
    if (next_state) {
        *current_state = next_state;
    }
}
