#include "scene.h"
#include "sokol_gfx.h"

#define VERTICE_LENGTH 7

typedef struct
{
    float *vertices;
    float vertices_length;
    float vertices_amount;
    sg_buffer buffer;
    sg_bindings bindings;
} Mesh;

Mesh make_mesh(float vertices[], int length)
{
    return (Mesh){
        .vertices = vertices,
        .vertices_length = length,
        .vertices_amount = length / VERTICE_LENGTH,
        .buffer = sg_make_buffer(&(sg_buffer_desc){
            .data = (sg_range){.ptr = vertices, .size = length}})};
}

void bind_mesh(Mesh *mesh)
{
    mesh->bindings = (sg_bindings){
        .vertex_buffers[0] = mesh->buffer};
}

void render_mesh(Mesh mesh, sg_pipeline pip)
{
    sg_apply_pipeline(pip);
    sg_apply_bindings(&mesh.bindings);
    sg_draw(0, mesh.vertices_amount, 1);
}

/* a vertex buffer */
const float vertices[] = {
    // positions            // colors
    0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f};

Mesh triangle;
sg_buffer vbuf;
sg_shader shd;
sg_pipeline pip;
sg_bindings bind;

void scene_setup()
{
    triangle = make_mesh(vertices, 21);
    bind_mesh(&triangle);

    shd = sg_make_shader(&(sg_shader_desc){
        .vs.source =
            "#version 330\n"
            "layout(location=0) in vec4 position;\n"
            "layout(location=1) in vec4 color0;\n"
            "out vec4 color;\n"
            "void main() {\n"
            "  gl_Position = position;\n"
            "  color = color0;\n"
            "}\n",
        .fs.source =
            "#version 330\n"
            "in vec4 color;\n"
            "out vec4 frag_color;\n"
            "void main() {\n"
            "  frag_color = color;\n"
            "}\n"});

    pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,
        .layout = {
            .attrs = {
                [0].format = SG_VERTEXFORMAT_FLOAT3,
                [1].format = SG_VERTEXFORMAT_FLOAT4}}});
}

void scene_draw()
{
    render_mesh(triangle, pip);
}