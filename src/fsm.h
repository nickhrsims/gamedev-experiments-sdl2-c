#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef unsigned int fsm_state_id_t;
typedef unsigned int fsm_trigger_id_t;

fsm_state_id_t fsm_get_target(size_t state_count, size_t trigger_count,
                              fsm_state_id_t fsm[state_count][trigger_count],
                              fsm_trigger_id_t trigger,
                              fsm_state_id_t current_state);

void fsm_send(size_t state_count, size_t trigger_count,
              fsm_state_id_t fsm[state_count][trigger_count],
              fsm_trigger_id_t trigger, fsm_state_id_t *current_state);
