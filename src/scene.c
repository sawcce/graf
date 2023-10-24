#include "scene.h"
#include "sokol_gfx.h"

#include "cglm/cglm.h"

#define VERTICE_LENGTH 7

typedef struct
{
    sg_range vertices;
    sg_buffer buffer;
    sg_bindings bindings;
} Mesh;

typedef struct
{
    vec3 position;
    vec3 scale;
    versor rotation;
    mat4 computed;
} Transform;

void rotate_euler(Transform *transform, vec3 euler)
{
    mat4 rotation_matrix;
    glm_euler_xyz(euler, rotation_matrix);

    versor new_rotation;
    glm_mat4_quat(rotation_matrix, new_rotation);
    glm_vec4_copy(new_rotation, transform->rotation);
}

void compute_transform(Transform *transform)
{
    mat4 rot = GLM_MAT4_IDENTITY_INIT, trans = GLM_MAT4_IDENTITY_INIT, scale = GLM_MAT4_IDENTITY_INIT;
    glm_translate(trans, transform->position);
    glm_scale_make(scale, transform->scale);
    glm_quat_mat4(transform->rotation, rot);

    mat4 computed;
    glm_mat4_mul(rot, scale, computed);
    glm_mat4_mul(trans, computed, computed);

    glm_mat4_copy(computed, transform->computed);
}

Mesh make_mesh(sg_range vertices)
{
    sg_buffer buff = sg_make_buffer(&(sg_buffer_desc){
        .data = vertices});

    return (Mesh){
        .vertices = vertices,
        .buffer = buff,
        .bindings = (sg_bindings){
            .vertex_buffers[0] = buff}};
}

void render_mesh(Mesh *mesh, sg_pipeline pip)
{
    sg_apply_bindings(&mesh->bindings);
    sg_draw(0, 3, 1);
}

/* a vertex buffer */
const float vertices[] = {
    // positions            // colors
    0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f};

Mesh triangle;

sg_shader shd;
sg_pipeline pip;

void scene_setup()
{
    triangle = make_mesh(SG_RANGE(vertices));

    shd = sg_make_shader(&(sg_shader_desc){
        .vs.source =
            "#version 330\n"
            "layout(location=0) in vec4 position;\n"
            "layout(location=1) in vec4 color0;\n"
            "uniform mat4 transform;\n"
            "out vec4 color;\n"
            "void main() {\n"
            "  gl_Position = transform * position;\n"
            "  color = color0;\n"
            "}\n",
        .vs.uniform_blocks[0] = {
            .size = sizeof(mat4),
            .uniforms = {
                [0] = {.name = "transform", .type = SG_UNIFORMTYPE_MAT4},
            },
        },
        .fs.source = "#version 330\n"
                     "in vec4 color;\n"
                     "out vec4 frag_color;\n"
                     "void main() {\n"
                     "  frag_color = color;\n"
                     "}\n",
    });

    pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,
        .layout = {
            .attrs = {
                [0].format = SG_VERTEXFORMAT_FLOAT3,
                [1].format = SG_VERTEXFORMAT_FLOAT4}}});
}

Transform transform = {
    .position = {0, 0.5f, 0},
    .scale = {1.5, 1, 1},
    .rotation = {0.3f, 0, 0, 0.7f},
};

float j = 0;

void scene_draw()
{
    rotate_euler(&transform, (vec3){0, j, 0});
    j += 0.01f;
    compute_transform(&transform);

    sg_apply_pipeline(pip);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &SG_RANGE(transform.computed));
    render_mesh(&triangle, pip);
}