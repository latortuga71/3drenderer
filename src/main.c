#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"
#include "vector.h"

// declare vectors/points for cube
const int N_POINTS = 9*9*9;
vec3_t cube_points[N_POINTS]; // 9x9x9 cube

void setup(void) {
    // window width and height
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * (window_width * window_height));
    if (color_buffer == NULL){
        fprintf(stderr,"main::setup::ERROR::Failed to allocate color buffer.\n");
        exit(-1);
    }
    // texture
    color_buffer_texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            window_width,
            window_height
    );
    int point_count = 0;
    // load array of vectors for cube
    // from -1 to 1 (in 9x9x9 cube)
    for (float x = -1; x <= 1; x += 0.25){
        for (float y = -1; y <= 1; y += 0.25){
            for (float z = -1; z <= 1; z +=0.25){
                vec3_t new_point = {.x = x, .y = y, .z = z};
                cube_points[point_count] = new_point;
                point_count++;
            }
        }
    }
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    // handle events
    switch(event.type){
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                is_running = false;
            break;
    }
}

void update(void) {
    /// to do
}


void render(void) {
    SDL_SetRenderDrawColor(renderer,255,0,0,255);
    SDL_RenderClear(renderer);
    draw_grid();
    draw_pixel(20,20,0xFFFFFF00);
    draw_rect(300,200,300,150,0xFFFF00FF);
    render_color_buffer();
    clear_color_buffer(0xFF000000);
    SDL_RenderPresent(renderer);
}

int main(int argc, char** argv){
    // create SDL Window
    is_running = init_window();
    setup();
    while(is_running){
        process_input();
        update();
        render();
    }
    destroy_window();
    return 0;
}
