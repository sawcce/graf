#pragma once

#include "sokol_gfx.h"
#include "transform.h"

typedef struct
{
    sg_range vertices;
    sg_bindings bindings;
    Transform *transform;
} Mesh;

Mesh make_mesh(sg_range vertices, sg_range indices, Transform *transform);
void render_mesh(Mesh *mesh, sg_pipeline pip);