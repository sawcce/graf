#include "mesh.h"

Mesh make_mesh(sg_range vertices, Transform *transform)
{
    sg_buffer buff = sg_make_buffer(&(sg_buffer_desc){
        .data = vertices});

    return (Mesh){
        .vertices = vertices,
        .buffer = buff,
        .transform = transform,
        .bindings = (sg_bindings){
            .vertex_buffers[0] = buff}};
}

void render_mesh(Mesh *mesh, sg_pipeline pip)
{
    sg_apply_pipeline(pip);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &SG_RANGE(mesh->transform->computed));
    sg_apply_bindings(&mesh->bindings);
    sg_draw(0, 3, 1);
}