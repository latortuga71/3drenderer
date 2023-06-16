#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "mesh.h"
#include "array.h"

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = {0,0,0},
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    { .x = -1, .y = -1, .z = -1}, // 1
    { .x = -1, .y =  1, .z = -1}, // 2
    { .x =  1, .y =  1, .z = -1}, // 3 
    { .x =  1, .y = -1, .z = -1}, // 4
    { .x =  1, .y =  1, .z = 1}, // 5
    { .x =  1, .y = -1, .z = 1}, // 6
    { .x = -1, .y =  1, .z = 1}, // 7
    { .x = -1, .y = -1, .z = 1}, // 8
};


face_t cube_faces[N_CUBE_FACES] = {

    {.a = 1, .b = 2, .c = 3, .color = 0xFFFF0000 },
    {.a = 1, .b = 3, .c = 4, .color = 0xFFFF0000 },
    
    {.a = 4, .b = 3, .c = 5, .color = 0xFF00FF00 },
    {.a = 4, .b = 5, .c = 6, .color = 0xFF00FF00 },

    {.a = 6, .b = 5, .c = 7, .color = 0xFF0000FF },
    {.a = 6, .b = 7, .c = 8, .color = 0xFF0000FF },

    {.a = 8, .b = 7, .c = 2, .color = 0xFFFFFF00 },
    {.a = 8, .b = 2, .c = 1, .color = 0xFFFFFF00 },

    {.a = 2, .b = 7, .c = 5, .color = 0xFFFF00FF },
    {.a = 2, .b = 5, .c = 3, .color = 0xFFFF00FF },

    {.a = 6, .b = 8, .c = 1, .color = 0xFF00FFFF },
    {.a = 6, .b = 1, .c = 4, .color = 0xFF00FFFF },


};

void load_cube_mesh_data(void){
    for (int i = 0; i < N_CUBE_VERTICES; i++){
        vec3_t cube_vertex = cube_vertices[i];
        array_push(mesh.vertices,cube_vertex);
    }
    for (int i = 0; i < N_CUBE_FACES; i++){
        face_t face = cube_faces[i];
        array_push(mesh.faces,face);
    }
}

void load_obj_file_data(char* filename){
    char* line_buffer = NULL;
    ssize_t nread;
    size_t line_length;
    // read obj file. load into mesh vertices in global mesh like above function.
    FILE* file = fopen(filename,"r");
    if (file == NULL){
        fprintf(stderr,"ERROR::load_obj_file_data::failed to open obj file %d\n",errno);
    }
    while((nread = getline(&line_buffer,&line_length,file)) != -1){
        if (strncmp(line_buffer,"v ",2) == 0){
            vec3_t vertex;
            sscanf(line_buffer,"v %f %f %f",&vertex.x,&vertex.y,&vertex.z);
            array_push(mesh.vertices,vertex);
        }
        if (strncmp(line_buffer,"f ",2) == 0){
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];
            sscanf(
                    line_buffer, "f %d/%d/%d %d/%d/%d %d/%d%d",
                    &vertex_indices[0],&texture_indices[0],&normal_indices[0],
                    &vertex_indices[1],&texture_indices[1],&normal_indices[1],
                    &vertex_indices[2],&texture_indices[2],&normal_indices[2]
                  );
            face_t face = {
                .a = vertex_indices[0],
                .b = vertex_indices[1],
                .c = vertex_indices[2]
            };
            array_push(mesh.faces,face);
        }
        /*starting_char = line_buffer[0];
        switch (starting_char){
            case 'v': {
                if (line_buffer[1] != ' ')
                    break;
                vec3_t vertex = parse_vector_from_line(&line_buffer[1]);
                //printf("%f %f %f\n",vertex.x,vertex.y,vertex.z);
                array_push(mesh.vertices,vertex);
                break;
            }
            case 'f':
                printf("%s",line_buffer);
                face_t face = parse_face_from_line(&line_buffer[1]);
                array_push(mesh.faces,face);
                break;
            default:
                break;
        }
        */
    }
    free(line_buffer);
    fclose(file);
}
