#include "mesh.h"

Mesh make_mesh(sg_range vertices, sg_range indices, Transform *transform)
{
    sg_buffer v_buff = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_VERTEXBUFFER,
        .data = vertices});

    sg_buffer i_buff = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .data = indices});

    return (Mesh){
        .vertices = vertices,
        .transform = transform,
        .bindings = (sg_bindings){
            .vertex_buffers[0] = v_buff,
            .index_buffer = i_buff,
        },
    };
}

void render_mesh(Mesh *mesh, sg_pipeline pip)
{
    sg_apply_pipeline(pip);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &SG_RANGE(mesh->transform->computed));
    sg_apply_bindings(&mesh->bindings);
    sg_draw(0, 3, 1);
}