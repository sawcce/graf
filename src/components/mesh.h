#pragma once

#include "sokol_gfx.h"

#include "transform.h"
#include "camera.h"

#define MESH_DEBUG

typedef struct
{
    int elements_amount;
    sg_range vertices;
    sg_range indices;
#ifdef MESH_DEBUG
    char *id;
#endif
} MeshData;

typedef struct
{
    MeshData mesh_data;
    sg_bindings bindings;
} Mesh;

Mesh make_mesh(MeshData mesh_data);
void render_mesh(Mesh *mesh, Transform *transform, Camera *viewer, Transform *camera_transform, sg_pipeline pip);