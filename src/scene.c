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

const int indices[] = {
    0,
    1,
    2,
};

MeshData triangle_data = {
    .vertices = SG_RANGE(vertices),
    .indices = SG_RANGE(indices),
};

Mesh triangle;
Mesh triangle2;

sg_shader shd;
sg_pipeline pip;

Transform transform = {
    .position = {0, 0.5f, 0},
    .scale = {0.5f, 0.5f, 0.5f},
    .rotation = {0.3f, 0, 0, 0.7f},
};

Transform transform2 = {
    .position = {0, 0.0f, 0},
    .scale = {0.5f, 0.5f, 0.5f},
    .rotation = {0.0f, 0, 0, 0.0f},
};

Scene scene;

void scene_setup()
{
    scene.entities = c_init(cmap_entity, {});

    Entity entity = {
        .components = {
            [0] = (Component){.type = CT_MESH, .component = &triangle},
            [1] = (Component){.type = CT_TRANSFORM, .component = &transform},
        },
    };

    Entity entity2 = {
        .components = {
            [0] = (Component){.type = CT_MESH, .component = &triangle2},
            [1] = (Component){.type = CT_TRANSFORM, .component = &transform2},
        },
    };

    triangle = make_mesh(triangle_data, &transform);
    cmap_entity_insert(&scene.entities, 0, entity);

    triangle2 = make_mesh(triangle_data, &transform2);
    cmap_entity_insert(&scene.entities, 1, entity2);

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
        .index_type = SG_INDEXTYPE_UINT32,
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
            Component component = entity.ref->second.components[i];
            if (component.type == CT_MESH)
                render_mesh(component.component, pip);
        }
    }
}

void scene_draw()
{
    rotate_euler(&transform, (vec3){j / 10.0f, j, 0});
    rotate_euler(&transform2, (vec3){j / 10.0f, j, 0});
    compute_transform(&transform);
    compute_transform(&transform2);

    j += 0.01f;

    scene_step();
}