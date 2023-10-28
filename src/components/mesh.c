#include "mesh.h"
#include "log.h"

Mesh make_mesh(MeshData mesh_data)
{
    sg_buffer v_buff = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_VERTEXBUFFER,
        .data = mesh_data.vertices});

    sg_buffer i_buff = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .data = mesh_data.indices});

    return (Mesh){
        .mesh_data = mesh_data,
        .bindings = (sg_bindings){
            .vertex_buffers[0] = v_buff,
            .index_buffer = i_buff,
        },
    };
}

typedef struct
{
    mat4 transform;
    mat4 perspective;
    mat4 view;
} Render_t;

void render_mesh(Mesh *mesh, Transform *transform, Camera *viewer, Transform *camera_transform, sg_pipeline pip)
{
    sg_apply_pipeline(pip);

    Render_t mr;
    glm_mat4_copy(transform->computed, mr.transform);
    glm_mat4_copy(viewer->perspective, mr.perspective);
    glm_quat_look(camera_transform->position, camera_transform->rotation, mr.view);

    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &SG_RANGE(mr));
    sg_apply_bindings(&mesh->bindings);
    sg_draw(0, mesh->mesh_data.elements_amount, 1);
}