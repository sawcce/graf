#pragma once

#include "sokol_gfx.h"
#include "transform.h"

typedef struct
{
    sg_range vertices;
    sg_range indices;
} MeshData;

typedef struct
{
    MeshData mesh_data;
    sg_bindings bindings;
    Transform *transform;
} Mesh;

Mesh make_mesh(MeshData mesh_data, Transform *transform);
void render_mesh(Mesh *mesh, sg_pipeline pip);