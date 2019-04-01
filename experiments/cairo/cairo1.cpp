
#include <cairo.h>

#include <SDL.h>

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return 1;

    SDL_Window *window = SDL_CreateWindow("Cairo",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,320,240,SDL_WINDOW_SHOWN);
    if (window == NULL)
        return 1;

    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (surface == NULL)
        return 1;

    cairo_format_t csurffmt = CAIRO_FORMAT_INVALID;
    if (surface->format->BytesPerPixel == 4) {
        csurffmt = CAIRO_FORMAT_ARGB32;
    }
    else if (surface->format->BytesPerPixel == 3) {
        csurffmt = CAIRO_FORMAT_RGB24;
    }
    else if (surface->format->BytesPerPixel == 2) {
        csurffmt = CAIRO_FORMAT_RGB16_565;
    }
    else {
        return 1;
    }

    cairo_surface_t* csurf = cairo_image_surface_create(csurffmt, 320, 240);
    if (csurf == NULL)
        return 1;

    cairo_t* cactx = cairo_create(csurf);
    if (cactx == NULL)
        return 1;

    bool run = true;
    while (run) {
        SDL_Event ev;

        if (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_WINDOWEVENT:
                    switch (ev.window.event) {
                        case SDL_WINDOWEVENT_CLOSE:
                            run = false;
                            break;
                    }
                    break;
                case SDL_KEYDOWN:
                    switch (ev.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            run = false;
                            break;
                    }
                    break;
            }
        }

        // clear
        cairo_reset_clip(cactx);
        cairo_set_source_rgba(cactx,0.25,0.25,0.25,1.0);
        cairo_paint(cactx);

        // copy Cairo output to display
        cairo_surface_flush(csurf);
        {
            unsigned char *cbuf = cairo_image_surface_get_data(csurf);
            int width = cairo_image_surface_get_width(csurf);
            int height = cairo_image_surface_get_height(csurf);
            int stride = cairo_image_surface_get_stride(csurf);

            SDL_LockSurface(surface);

            if (cbuf != NULL && surface->pixels != NULL) {
                if (width > surface->w) width = surface->w;
                if (height > surface->h) height = surface->h;

                for (int y=0;y < height;y++) {
                    unsigned char *d = (unsigned char*)(surface->pixels) + (surface->pitch * y);
                    unsigned char *s = (unsigned char*)(cbuf) + (stride * y);
                    memcpy(d,s,width*surface->format->BytesPerPixel);
                }
            }

            SDL_UnlockSurface(surface);
            SDL_UpdateWindowSurface(window);
        }
    }

    cairo_surface_flush(csurf);
    cairo_destroy(cactx);
    cairo_surface_destroy(csurf);

    // do not free surface, owned by dinwo
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

