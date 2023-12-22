#include <SDL2/SDL.h>
#include <SDL_ttf.h>

#include "alloc.h"
#include "video.h"

// -----------------------------------------------------------------------------
// Static SDL2 Resources
// -----------------------------------------------------------------------------

typedef struct video_s {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
} video_t;

video_t *video_init(video_cfg_t *config) {
    video_t *v = new (video_t);

    SDL_InitSubSystem(SDL_INIT_VIDEO);

    // --- Window
    v->window = SDL_CreateWindow(
        config->window_title, config->window_position_x, config->window_position_y,
        config->window_width, config->window_height,
        config->window_is_fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

    // --- Renderer
    // No use for variable index or flags.
    static unsigned char const RENDERER_INDEX = 0;
    static unsigned char const RENDERER_FLAGS = 0;

    v->renderer = SDL_CreateRenderer(v->window, RENDERER_INDEX, RENDERER_FLAGS);

    // --- Font
    // TODO: Generalize font selection
    TTF_Init();
    v->font = TTF_OpenFont("res/font.ttf", 24);

    return v;
}

void video_term(video_t *v) {
    TTF_CloseFont(v->font);
    SDL_DestroyRenderer(v->renderer);
    SDL_DestroyWindow(v->window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

/**
 * Clear the screen.
 */
void video_clear(video_t *v) {
    SDL_RenderClear(v->renderer);
    return;
}

/**
 * Render all drawn elements.
 */
void video_render(video_t *v) {
    SDL_RenderPresent(v->renderer);
    return;
}

/**
 * Set draw color.
 */
void video_set_color(video_t *v, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(v->renderer, r, g, b, a);
    return;
}

/**
 * Reset entity draw color (set color to black).
 */
void video_reset_color(video_t *g) {
    video_set_color(g, 0, 0, 0, 0);
    return;
}

/**
 * Draw specific axis-aligned region
 */
void video_draw_region(video_t *v, int x, int y, int w, int h) {
    SDL_RenderFillRect(v->renderer, &(SDL_Rect){.x = x, .y = y, .w = w, .h = h});
    return;
}

/**
 * Draw text.
 */
void video_draw_text(video_t *v, char *str, int x, int y) {
    SDL_Surface *surface =
        TTF_RenderText_Solid(v->font, str, (SDL_Color){255, 255, 255, 255});

    SDL_Texture *texture = SDL_CreateTextureFromSurface(v->renderer, surface);

    SDL_RenderCopy(v->renderer, texture, NULL,
                   &(SDL_Rect){x, y, surface->w, surface->h});
}

void video_get_window_size(video_t *v, int *w, int *h) {
    SDL_GetWindowSize(v->window, w, h);
}
