#pragma once

#include "sokol_gfx.h"
#include "transform.h"

typedef struct
{
    sg_range vertices;
    sg_buffer buffer;
    sg_bindings bindings;
    Transform *transform;
} Mesh;

Mesh make_mesh(sg_range vertices, Transform *transform);
void render_mesh(Mesh *mesh, sg_pipeline pip);