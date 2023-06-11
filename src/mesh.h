#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6*2) // 6 faces 2 triangles per face

extern vec3_t cube_vertices[N_CUBE_VERTICES];
extern face_t cube_faces[N_CUBE_FACES];

//// mesh structs;

typedef struct {
    vec3_t* vertices; // array of vertices 
    face_t* faces; // array of faces;
    vec3_t rotation; // rotation information for the mesh x,y,z
} mesh_t;

void load_cube_mesh_data(void);

void load_obj_file_data(char* filename);
extern mesh_t mesh;

#endif
