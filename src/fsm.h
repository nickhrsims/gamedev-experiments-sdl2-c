#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uint8_t fsm_state_id_t;
typedef uint8_t fsm_trigger_id_t;

fsm_state_id_t fsm_get_target(fsm_state_id_t **fsm, fsm_trigger_id_t trigger,
                              fsm_state_id_t current_state);
void fsm_send(fsm_state_id_t **fsm, fsm_trigger_id_t trigger,
              fsm_state_id_t *current_state);
