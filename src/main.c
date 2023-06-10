#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"

triangle_t triangles_to_render[N_MESH_FACES];

vec3_t camera_position = { .x = 0, .y = 0, .z = -5};
vec3_t cube_rotation = { .x = 0, .y = 0, .z = 0 };

int previous_frame_time = 0;

//float fov_factor = 128;
float fov_factor = 640;

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

// 3d vector return projected 2d point
vec2_t project(vec3_t point){
    vec2_t projected_point = {
        .x = (fov_factor * point.x) / point.z,
        .y = (fov_factor * point.y) / point.z,
    };
    return projected_point;
}


void update(void) {
    // waste cycles until we reach our frame target time
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
        SDL_Delay(time_to_wait);
    }
    // reached target time, update frame and render now continue
    previous_frame_time = SDL_GetTicks(); 
    cube_rotation.y += 0.01;
    cube_rotation.x += 0.01;
    cube_rotation.z += 0.01;
    for (int i = 0; i < N_MESH_FACES; i++){
        face_t mesh_face = mesh_faces[i];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh_vertices[mesh_face.a  - 1 ];
        face_vertices[1] = mesh_vertices[mesh_face.b  - 1 ];
        face_vertices[2] = mesh_vertices[mesh_face.c  - 1 ];
        triangle_t projected_triangle;
        // loop all three vertices of current face and apply transformations 
        for (int j = 0; j < 3; j++){
            vec3_t transformed_vertex =  face_vertices[j];
            transformed_vertex = vec3_rotate_x(transformed_vertex,cube_rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex,cube_rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex,cube_rotation.z);
            //transformed vertex
            
            // move camera away
            transformed_vertex.z -= camera_position.z;

            vec2_t projected_point = project(transformed_vertex);
            // scale and translate projected popint
            projected_point.x += (window_width / 2);
            projected_point.y += (window_height / 2);
            projected_triangle.points[j] = projected_point;
        }
        // save the projected triangle in the array of triangles to render.
        triangles_to_render[i] = projected_triangle;

    }
    /*
    for (int i = 0; i < N_POINTS; i++){
        vec3_t point = cube_points[i];
        // rotate point
        //vec3_t transformed_point = vec3_rotate_y(point,cube_rotation.y);
        vec3_t transformed_point = vec3_rotate_x(point,cube_rotation.x);
        transformed_point = vec3_rotate_y(transformed_point,cube_rotation.y);
        //transformed_point = vec3_rotate_x(transformed_point,cube_rotation.x);
        transformed_point = vec3_rotate_z(transformed_point,cube_rotation.z);
        // translate move point away from camera
        transformed_point.z -= camera_position.z;
        // project current point
        vec2_t projected_point = project(transformed_point);
        // save point
        projected_points[i] = projected_point;
    }*/
}


void render(void) {
    draw_grid();
    for (int i = 0; i < N_MESH_FACES; i++){
        triangle_t triangle = triangles_to_render[i];
        // draw vertexx points
        draw_rect(triangle.points[0].x,triangle.points[0].y,3,3,0xFFFFFF00);
        draw_rect(triangle.points[1].x,triangle.points[1].y,3,3,0xFFFFFF00);
        draw_rect(triangle.points[2].x,triangle.points[2].y,3,3,0xFFFFFF00);
        // draw unfilled triangle
        draw_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y,
                0xFFFFFFF00
                );
    }
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

    //SDL_SetRenderDrawColor(renderer,255,0,0,255);
    //SDL_RenderClear(renderer);
    //draw_grid();
    // loop all projected points and render them
    // #include "mesh.h"
    //
    /*
    for (int i = 0; i < N_POINTS; i++){
        vec2_t projected_point = projected_points[i];
        draw_rect(projected_point.x + (window_width / 2) ,projected_point.y + (window_height / 2),4,4,0xFFFFFF00);
    }
    //draw_pixel(20,20,0xFFFFFF00);
    //draw_rect(300,200,300,150,0xFFFF00FF);
    //*/
