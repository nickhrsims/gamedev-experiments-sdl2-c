#include <SDL2/SDL.h>
#include <SDL_ttf.h>

#include "log.h"

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
    video_t *v  = new (video_t);
    v->window   = NULL;
    v->renderer = NULL;
    v->font     = NULL;

    if (SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        log_error(SDL_GetError());
        return NULL;
    }

    // --- Window
    if (!(v->window = SDL_CreateWindow(
              config->window_title, config->window_position_x,
              config->window_position_y, config->window_width, config->window_height,
              config->window_is_fullscreen ? SDL_WINDOW_FULLSCREEN : 0))) {
        log_error(SDL_GetError());
        log_error("Cannot create window");
        return NULL;
    }

    // --- Renderer
    // No use for variable index or flags.
    static unsigned char const RENDERER_INDEX = 0;
    static unsigned char const RENDERER_FLAGS = 0;

    if (!(v->renderer =
              SDL_CreateRenderer(v->window, RENDERER_INDEX, RENDERER_FLAGS))) {
        video_term(v);
        log_error(SDL_GetError());
        return NULL;
    }

    if (SDL_SetRenderDrawBlendMode(v->renderer, SDL_BLENDMODE_BLEND)) {
        video_term(v);
        log_error(SDL_GetError());
        return NULL;
    }

    // --- Font
    // TODO: Generalize font selection
    if (TTF_Init() < 0) {
        video_term(v);
        log_error(TTF_GetError());
        return NULL;
    }

    if (!(v->font = TTF_OpenFont("res/font.ttf", 24))) {
        video_term(v);
        log_error(TTF_GetError());
        return NULL;
    }

    return v;
}

void video_term(video_t *v) {
    if (!v) {
        return;
    }
    TTF_CloseFont(v->font);
    SDL_DestroyRenderer(v->renderer);
    SDL_DestroyWindow(v->window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    delete (v);
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
void video_draw_region(video_t *v, aabb_t *region) {
    SDL_RenderFillRect(v->renderer, region);
    return;
}

/**
 * Draw text with specified color.
 *
 * TODO: Make text elements first-class citizens with texture properties.
 */
void video_draw_text_with_color(video_t *v, char *str, int x, int y, uint8_t r,
                                uint8_t g, uint8_t b, uint8_t a) {
    SDL_Surface *surface = TTF_RenderText_Solid(v->font, str, (SDL_Color){r, g, b, a});

    SDL_Texture *texture = SDL_CreateTextureFromSurface(v->renderer, surface);

    SDL_RenderCopy(v->renderer, texture, NULL,
                   &(SDL_Rect){
                       x - (surface->w / 2),
                       y - (surface->h / 2),
                       surface->w,
                       surface->h,
                   });

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

/**
 * Draw text.
 */
void video_draw_text(video_t *v, char *str, int x, int y) {
    video_draw_text_with_color(v, str, x, y, 255, 255, 255, 255);
}

void video_get_window_size(video_t *v, int *w, int *h) {
    SDL_GetWindowSize(v->window, w, h);
}
