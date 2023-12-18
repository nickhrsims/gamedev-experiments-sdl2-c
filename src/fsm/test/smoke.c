#include <stdio.h>
#include <stdlib.h>

#include "fsm/fsm.h"

#include "test.h"

static int counter = 0;

void start_activity(fsm_t *fsm, void *ctx) {
    int *c = (int *)ctx;
    *c += 1;
    fsm_trigger(fsm, NEXT);
}
void one_activity(fsm_t *fsm, void *ctx) {
    int *c = (int *)ctx;
    *c += 1;
    fsm_trigger(fsm, NEXT);
}
void two_activity(fsm_t *fsm, void *ctx) {
    int *c = (int *)ctx;
    *c += 1;
    printf("Counter is %d\n", *c);
    fsm_trigger(fsm, NEXT);
}
void stop_activity(fsm_t *fsm, void *ctx) {
    (void)fsm;
    (void)ctx;
}

int main(void) {
    // init
    fsm_t *fsm = fsm_init(STATE_COUNT, TRIGGER_COUNT, START);

    // activities
    fsm_set_activity(fsm, START, start_activity, &counter);
    fsm_set_activity(fsm, ONE, one_activity, &counter);
    fsm_set_activity(fsm, TWO, two_activity, &counter);
    fsm_set_activity(fsm, STOP, stop_activity, NULL);

    // start
    fsm_on(fsm, START, NEXT, ONE);
    fsm_on(fsm, START, PREV, NOT_ALLOWED);

    // one
    fsm_on(fsm, ONE, NEXT, TWO);
    fsm_on(fsm, ONE, PREV, START);

    // two
    fsm_on(fsm, TWO, NEXT, STOP);
    fsm_on(fsm, TWO, PREV, ONE);

    // stop
    // NONE

    while (fsm_state(fsm) != STOP) {
        fsm_do_activity(fsm);
    }

    fsm_term(fsm);
}
