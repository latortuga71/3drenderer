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

    {.a = 1, .b = 2, .c = 3},
    {.a = 1, .b = 3, .c = 4},
    
    {.a = 4, .b = 3, .c = 5},
    {.a = 4, .b = 5, .c = 6},

    {.a = 6, .b = 5, .c = 7},
    {.a = 6, .b = 7, .c = 8},

    {.a = 8, .b = 7, .c = 2},
    {.a = 8, .b = 2, .c = 1},

    {.a = 2, .b = 7, .c = 5},
    {.a = 2, .b = 5, .c = 3},

    {.a = 6, .b = 8, .c = 1},
    {.a = 6, .b = 1, .c = 4},


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

vec3_t parse_vector_from_line(char* line){
    char* token = strtok(line," ");
    if (token == NULL){
        fprintf(stderr,"ERROR::parse_vector_from_line::failed to parse float %s\n",token);
        exit(-1);
    }
    float x = atof(token);
    token = strtok(NULL," ");
    if (token == NULL){
        fprintf(stderr,"ERROR::parse_vector_from_line::failed to parse float %s\n",token);
        exit(-1);
    }
    float y = atof(token);
    token = strtok(NULL," ");
    if (token == NULL){
        fprintf(stderr,"ERROR::parse_vector_from_line::failed to parse float %s\n",token);
        exit(-1);
    }
    float z = atof(token);
    vec3_t vertex = {.x = x, .y = y, .z = z};
    return vertex;
}

face_t parse_face_from_line(char* line){
    char* token = strtok(line," ");
    if (token == NULL){
        fprintf(stderr,"ERROR::parse_face_from_line::failed to parse float\n");
        exit(-1);
    }
    printf("token -> #%s \n",token);
    int a = token[0] - '0';
    if (a == 0){
        fprintf(stderr,"ERROR::parse_face_from_line::failed to parse float\n");
        exit(-1);
    }
    token = strtok(NULL," ");
    if (token == NULL){
        fprintf(stderr,"ERROR::parse_face_from_line::failed to parse float\n");
        exit(-1);
    }
    printf("token -> #%s \n",token);
    int b = token[0] - '0';
    if (b == 0){
        fprintf(stderr,"ERROR::parse_face_from_line::failed to parse float\n");
        exit(-1);
    }
    token = strtok(NULL," ");
    if (token == NULL){
        fprintf(stderr,"ERROR::parse_face_from_line::failed to parse float\n");
        exit(-1);
    }
    printf("token -> #%s \n",token);
    int c = token[0] - '0';
    if (c == 0){
        fprintf(stderr,"ERROR::parse_face_from_line::failed to parse float\n");
        exit(-1);
    }
    printf("a:%d b:%d c:%d\n",a,b,c);
    face_t face = {
        .a = a,
        .b = b,
        .c = c,
    }; 
    return face;
}

void load_obj_file_data(char* filename){
    char* line_buffer = NULL;
    ssize_t nread;
    size_t line_length;
    char starting_char;
    // read obj file. load into mesh vertices in global mesh like above function.
    FILE* file = fopen(filename,"r");
    if (file == NULL){
        fprintf(stderr,"ERROR::load_obj_file_data::failed to open obj file %d\n",errno);
    }
    while((nread = getline(&line_buffer,&line_length,file)) != -1){
        starting_char = line_buffer[0];
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
    }
    free(line_buffer);
    fclose(file);
}
