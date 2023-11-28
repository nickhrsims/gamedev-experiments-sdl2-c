#include <assert.h>
#include <stdlib.h>

#include "fsm.h"

fsm_state_id_t fsm_get_target(fsm_state_table_t fsm, fsm_trigger_id_t trigger,
                              fsm_state_id_t current_state) {
  assert(fsm);
  assert(fsm[current_state]);
  return fsm[current_state][trigger];
}

void fsm_send(fsm_state_table_t fsm, fsm_trigger_id_t trigger,
              fsm_state_id_t *current_state) {
  assert(fsm);
  assert(fsm[current_state]);
  fsm_state_id_t next_state = fsm[*current_state][trigger];
  if (next_state)
    *current_state = next_state;
}
