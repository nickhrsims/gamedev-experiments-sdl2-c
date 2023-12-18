#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct fsm_s fsm_t;
typedef void (*fsm_activity_t)(fsm_t *fsm, void *context);

fsm_t *fsm_init(size_t state_count, size_t trigger_count, int start_state);
void fsm_term(fsm_t *fsm);

void fsm_on(fsm_t *fsm, int from, int trigger, int to);
void fsm_set_activity(fsm_t *fsm, int state, fsm_activity_t activity,
                      void *context);
void fsm_do_activity(fsm_t *fsm);
void fsm_trigger(fsm_t *fsm, int trigger);
int fsm_state(fsm_t *fsm);
