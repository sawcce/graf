#include "scene.h"

#include "cglm/cglm.h"

#include "components/mesh.h"
#include "components/transform.h"
#include "ecs.h"

/* a vertex buffer */
const float vertices[] = {
    // positions            // colors
    0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f};

Mesh triangle;

sg_shader shd;
sg_pipeline pip;

Transform transform = {
    .position = {0, 0.5f, 0},
    .scale = {1.5, 1, 1},
    .rotation = {0.3f, 0, 0, 0.7f},
};

Scene scene;

void scene_setup()
{
    scene.entities = c_init(cmap_entity, {});

    Entity entity = {
        .components = {
            [0] = (Component){.type = CT_MESH, .component = &triangle},
        },
    };

    triangle = make_mesh(SG_RANGE(vertices), &transform);
    cmap_entity_insert(&scene.entities, 0, entity);

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

float j = 0;

void scene_step()
{
    c_foreach(entity, cmap_entity, scene)
    {
        for (int i = 0; i < 3; i++)
        {
            Component component = entity.ref->second.components[0];
            if (component.type == CT_MESH)
                render_mesh(component.component, pip);
        }
    }
}

void scene_draw()
{
    rotate_euler(&transform, (vec3){0, j, 0});
    j += 0.01f;
    compute_transform(&transform);
    scene_step();
}