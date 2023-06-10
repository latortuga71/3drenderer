#include "display.h"

// display globals
bool is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;
uint32_t* color_buffer = NULL;
int window_width = 800;
int window_height = 600;

// window
bool init_window(void){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        fprintf(stderr,"main::init_window::ERROR::Initializing SDL.\n");
        return false;
    }
    // create window
    window = SDL_CreateWindow(
            NULL,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            window_width,
            window_height,
            SDL_WINDOW_BORDERLESS
            );
    if (window == NULL){
        fprintf(stderr,"main::init_window::ERROR::Initializing Window.\n");
        return false;
    }
    // create SDL Renderer
    renderer = SDL_CreateRenderer(window,-1,0);
    if (renderer == NULL){
        fprintf(stderr,"main::init_window::ERROR::Initializing Renderer.\n");
        return false;
    }
    return true;
}

void destroy_window(void){
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// render functions
void render_color_buffer(void){
    // copy color_buffer to texture
    SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer, (int)(window_width * sizeof(uint32_t)));
    // render texture
    SDL_RenderCopy(renderer,color_buffer_texture,NULL,NULL);
}

void clear_color_buffer(uint32_t color){
    for (int y = 0; y < window_height; y++){
        for (int x = 0; x < window_width; x++){
            color_buffer[(window_width * y) + x] = color;
        }
    }
}


// draw functions
void draw_rect(int x, int y, int width, int height, uint32_t color){
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            int current_x = x + i;
            int current_y = y + j;
            draw_pixel(current_x,current_y,color);
        }
    }
}

void draw_grid(void){
    for (int row = 0; row < window_height; row += 10){
        for (int col = 0; col < window_width; col += 10){
            color_buffer[(window_width * row) + col] = 0x00000000;
            color_buffer[(window_width * row) + col] = 0x00000000;
        }
    }
}

void draw_pixel(int x, int y, uint32_t color){
    if (x >=0 && x < window_width && y >=0 && y < window_height){
        color_buffer[(window_width * y) + x] = color;
    }
}

