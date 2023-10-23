#include "scene.h"
#include "sokol_gfx.h"

/* a vertex buffer */
const float vertices[] = {
    // positions            // colors
    0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f};

sg_buffer vbuf;
sg_shader shd;
sg_pipeline pip;
sg_bindings bind;

void scene_setup()
{
    vbuf = sg_make_buffer(&(sg_buffer_desc){
        .data = SG_RANGE(vertices)});

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

    bind = (sg_bindings){
        .vertex_buffers[0] = vbuf};
}

void scene_draw()
{
    sg_apply_pipeline(pip);
    sg_apply_bindings(&bind);
    sg_draw(0, 3, 1);
}