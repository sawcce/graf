#include "scene.h"

#include "cglm/cglm.h"

#include "components/mesh.h"
#include "components/transform.h"
#include "ecs.h"
#include "log.h"

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
    .elements_amount = 3,
    .id = "triangular",
};

Mesh triangle;

sg_shader shd;
sg_pipeline pip;

Transform transform = {
    .position = {0, 0.5f, 0},
    .scale = {0.5f, 0.5f, 0.5f},
};

Transform transform2 = {
    .position = {0, 0.0f, 0},
    .scale = {0.5f, 0.5f, 0.5f},
};

Scene *scene;

void scene_setup()
{
    triangle = make_mesh(triangle_data);

    scene = new_scene();

    const int iter = 5000;
    for (int i = 0; i < 10000; i++)
    {
        const float pos = ((float)i / ((float)iter / 2.0f)) - 1.0f;
        Transform transform = {
            .position = {pos, pos, 0},
            .scale = {0.1f, 0.1f, 0.1f},
        };

        EntityID entity = new_entity(scene);
        assign_to_entity(scene, entity, CT_MESH, sizeof(Mesh), &triangle);
        assign_to_entity(scene, entity, CT_TRANSFORM, sizeof(Transform), &transform);
    }

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

void mesh_system()
{
    c_forpair(entity, type_set, cmap_entities, scene->entities)
    {
        // log_debug("Entity ( %d ) with TS of size: %d", *_.entity, _.type_set->size);

        if (TypeSet_get(_.type_set, CT_MESH) == NULL || TypeSet_get(_.type_set, CT_TRANSFORM) == NULL)
            continue;

        Mesh *mesh = get_component_for_entity(scene, *_.entity, CT_MESH);
        Transform *transform = get_component_for_entity(scene, *_.entity, CT_TRANSFORM);

        rotate_euler(transform, (vec3){j / 10.0f, j, 0});
        compute_transform(transform);

        render_mesh(mesh, transform, pip);
    }
}

void scene_draw()
{
    j += 0.01f;

    mesh_system();
}