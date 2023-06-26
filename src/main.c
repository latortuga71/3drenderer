#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"
#include "matrix.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "triangle.h"

// array of triangles to render frame by frame
//triangle_t triangles_to_render[N_MESH_FACES];
triangle_t* triangles_to_render = NULL;

vec3_t camera_position = { .x = 0, .y = 0, .z = 0 };
//vec3_t mesh.rotation = { .x = 0, .y = 0, .z = 0 };

int previous_frame_time = 0;

//float fov_factor = 128;
float fov_factor = 640;

void setup(void) {

    render_method = RENDER_WIRE;
    cull_method = CULL_BACKFACE;

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
    // load the mesh data 
    load_cube_mesh_data();
    //load_obj_file_data("assets/cube.obj");
    //exit(0);
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
            if (event.key.keysym.sym == SDLK_1)
                render_method = RENDER_WIRE_VERTEX;
            if (event.key.keysym.sym == SDLK_2)
                render_method = RENDER_WIRE;
            if (event.key.keysym.sym == SDLK_3)
                render_method = RENDER_FILL_TRIANGLE;
            if (event.key.keysym.sym == SDLK_4)
                render_method = RENDER_FILL_TRIANGLE_WIRE;
            if (event.key.keysym.sym == SDLK_c)
                cull_method = CULL_BACKFACE;
            if (event.key.keysym.sym == SDLK_d)
                cull_method = CULL_NONE;
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
    // init array of triangles to render 
    triangles_to_render = NULL;

    mesh.rotation.y += 0.01;
    mesh.rotation.x += 0.01;
    mesh.rotation.z += 0.01;

    mesh.scale.x += 0.002;
    mesh.scale.y += 0.001;
    //mesh.translation.x += 0.01;
    // translate vertex away from camera 5 units inside monitor
    mesh.translation.z = 5; 

    // Create scale rotation ,translation matrix used to multiply mesh vertices
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);

    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    // mesh elements faces etc.
    int num_faces = array_length(mesh.faces);
    // for all faces
    for (int i = 0; i < num_faces; i++){
        face_t mesh_face = mesh.faces[i];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a  - 1 ];
        face_vertices[1] = mesh.vertices[mesh_face.b  - 1 ];
        face_vertices[2] = mesh.vertices[mesh_face.c  - 1 ];


        vec4_t transformed_vertices[3];
        //vec3_t transformed_vertices[3];

        // loop all three vertices of current face and apply transformations 
        for (int j = 0; j < 3; j++){
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            mat4_t world_matrix = mat4_identity();

            // order important scale then rotate then translate. order of operation with matrix multiplication matters
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);

            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);

            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            /*
            transformed_vertex = mat4_mul_vec4(scale_matrix,transformed_vertex);
            transformed_vertex = mat4_mul_vec4(rotation_matrix_x,transformed_vertex);
            transformed_vertex = mat4_mul_vec4(rotation_matrix_y,transformed_vertex);
            transformed_vertex = mat4_mul_vec4(rotation_matrix_z,transformed_vertex);
            transformed_vertex = mat4_mul_vec4(translation_matrix,transformed_vertex);
            */
            transformed_vertices[j] = transformed_vertex;
        }
        if (cull_method == CULL_BACKFACE){
            vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
            vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
            vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);
            //vec3_t vector_a = vectransformed_vertices[0];
            //vec3_t vector_b = transformed_vertices[1];
            //vec3_t vector_c = transformed_vertices[2];
            vec3_t vector_ab = vec3_sub(vector_b,vector_a);
            vec3_t vector_ac = vec3_sub(vector_c,vector_a);
            vec3_normalize(&vector_ab);
            vec3_normalize(&vector_ac);
            // get face normal using cross product 
            // below order of variables matters because of the handed ness of z axis
            vec3_t normal = vec3_cross(vector_ab,vector_ac);
            vec3_normalize(&normal);
            // normalized normal vector
            // find vector between point in the triangle and the camera origin
            vec3_t camera_ray = vec3_sub(camera_position,vector_a);
            // check if camera ray is aligned to normal negative or positive
            float dot_normal_camera = vec3_dot(normal,camera_ray);
            // if camera alignment is negative dont render this face.
            if (dot_normal_camera < 0) {
                continue;
            }
        }
        vec2_t projected_points[3];
        // projecting points
        for (int j = 0; j < 3; j++){
            // project all points
            projected_points[j] = project(vec3_from_vec4(transformed_vertices[j]));
            //projected_points[j] = project(transformed_vertices[j]);

            // scale and translate projected popint
            projected_points[j].x += (window_width / 2);
            projected_points[j].y += (window_height / 2);
            //projected_triangle.points[j] = projected_point;
        }
        // calculate average depth for each face based on vertices after transformation z values
        float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;
        triangle_t projected_triangle = {
            .points = {
                { projected_points[0].x, projected_points[0].y},
                { projected_points[1].x, projected_points[1].y},
                { projected_points[2].x, projected_points[2].y},
            },
            .color = mesh_face.color,
            .avg_depth = avg_depth
        };
        // save the projected triangle in the array of triangles to render.
        //triangles_to_render[i] = projected_triangle;
        array_push(triangles_to_render,projected_triangle);
    }
    // sort triangles to render by their average depth ascending.
    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++){
        for (int j = 0; j < num_triangles; j++){
            if (triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth){
                triangle_t temp = triangles_to_render[i];
                triangles_to_render[i] = triangles_to_render[j];
                triangles_to_render[j] = temp;
            }
        }
    }
}


void render(void) {
    draw_grid();
    // how many triangles in dynamic array
    // triangles should be sorted by depth
    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++){
        triangle_t triangle = triangles_to_render[i];
        if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE){
            draw_filled_triangle(
                    triangle.points[0].x,
                    triangle.points[0].y,
                    triangle.points[1].x,
                    triangle.points[1].y,
                    triangle.points[2].x,
                    triangle.points[2].y,
                    triangle.color
                    );
        }
        if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE){
            draw_triangle(
                    triangle.points[0].x,
                    triangle.points[0].y,
                    triangle.points[1].x,
                    triangle.points[1].y,
                    triangle.points[2].x,
                    triangle.points[2].y,
                    0xFFFFFAA00
                    );
        }
        if (render_method == RENDER_WIRE_VERTEX){
            draw_rect(triangle.points[0].x - 3,triangle.points[0].y - 3,6,6,0xFF0FF00);
            draw_rect(triangle.points[1].x - 3 ,triangle.points[1].y - 3,6,6,0xFF0FFF00);
            draw_rect(triangle.points[2].x - 3 ,triangle.points[2].y - 3,6,6,0xFF0FFF00);
        }

    }
    // clear array of triangles every frame
    array_free(triangles_to_render);
    render_color_buffer();
    clear_color_buffer(0xFF000000);
    SDL_RenderPresent(renderer);
}

void free_resources(void){
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
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
