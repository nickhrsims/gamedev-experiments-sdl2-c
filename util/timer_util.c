#include "SDL_timer.h"

#include "timer.h"

unsigned long timer_get_ms(void) { return SDL_GetTicks64(); }
unsigned long timer_get_elapsed_ms(unsigned long last_ms) {
    return SDL_GetTicks64() - last_ms;
}
