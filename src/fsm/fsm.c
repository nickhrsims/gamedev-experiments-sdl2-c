#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "fsm.h"

typedef struct fsm_state_s {
    fsm_activity_t activity;
    void *context;
} fsm_state_data_t;

typedef struct fsm_s {
    int current_state;
    int state_count;
    int trigger_count;
    int *transitions;
    fsm_state_data_t *states;
} fsm_t;

// -----------------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------------

static fsm_t *_root_fsm_init(size_t state_count, size_t trigger_count,
                             int start_state) {
    fsm_t *fsm         = malloc(sizeof(fsm_t));
    fsm->state_count   = state_count;
    fsm->trigger_count = trigger_count;
    fsm->current_state = start_state;
    return fsm;
}

static int *_transition_table_init(size_t state_count, size_t trigger_count) {
    int *transitions = calloc(state_count * trigger_count, sizeof(int));
    return transitions;
}

static fsm_state_data_t *_states_init(size_t state_count) {
    fsm_state_data_t *states = malloc(sizeof(fsm_state_data_t) * state_count);
    return states;
}

fsm_t *fsm_init(size_t state_count, size_t trigger_count, int start_state) {
    fsm_t *fsm       = _root_fsm_init(state_count, trigger_count, start_state);
    fsm->transitions = _transition_table_init(state_count, trigger_count);
    fsm->states      = _states_init(state_count);
    return fsm;
}

// -----------------------------------------------------------------------------
// Termination
// -----------------------------------------------------------------------------

static void _states_term(fsm_state_data_t *states) { free(states); }

static void _transition_table_term(int *transitions) { free(transitions); }

static void _root_fsm_term(fsm_t *fsm) { free(fsm); }

void fsm_term(fsm_t *fsm) {
    _states_term(fsm->states);
    _transition_table_term(fsm->transitions);
    _root_fsm_term(fsm);
}

// -----------------------------------------------------------------------------
// Transitions
// -----------------------------------------------------------------------------

void fsm_on(fsm_t *fsm, int from, int trigger, int to) {
    *(fsm->transitions + (from * fsm->trigger_count) + trigger) = to;
}

// -----------------------------------------------------------------------------
// Activities
// -----------------------------------------------------------------------------

void fsm_set_activity(fsm_t *fsm, int state, fsm_activity_t activity, void *context) {
    fsm_state_data_t *state_data = (fsm->states + state);
    state_data->activity         = activity;
    state_data->context          = context;
}

void fsm_do_activity(fsm_t *fsm) {
    fsm_activity_t activity = *(fsm->states + fsm->current_state)->activity;
    if (!activity)
        return;
    void *context = (fsm->states + fsm->current_state)->context;
    activity(fsm, context);
}

// -----------------------------------------------------------------------------
// Trigger
// -----------------------------------------------------------------------------

void fsm_trigger(fsm_t *fsm, int trigger) {
    int next_state =
        *(fsm->transitions + (fsm->current_state * fsm->trigger_count) + trigger);
    if (next_state) {
        fsm->current_state = next_state;
    };
}

// -----------------------------------------------------------------------------
// Queries
// -----------------------------------------------------------------------------

int fsm_state(fsm_t *fsm) { return fsm->current_state; }
