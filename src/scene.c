#include "scene.h"

#include "cglm/cglm.h"

#include "components/mesh.h"
#include "components/transform.h"
#include "components/camera.h"

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

    EntityID camera = new_entity(scene);
    assign_to_entity(scene, camera, CT_CAMERA, sizeof(Camera), &(Camera){
                                                                   .active = true,
                                                                   .fov = (70.0f / 360.0f) * 2 * GLM_PI,
                                                                   .aspect_ratio = 4.0f / 3.0f,
                                                               });

    assign_to_entity(scene, camera, CT_TRANSFORM, sizeof(Transform), &(Transform){
                                                                         .position = {0, 0, -1},
                                                                     });

    const int iter = 5000;
    for (int i = 0; i < iter; i++)
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
            "uniform mat4 perspective;\n"
            "uniform mat4 view;\n"
            "out vec4 color;\n"
            "void main() {\n"
            "  gl_Position = perspective * view * transform * position;\n"
            "  color = color0;\n"
            "}\n",
        .vs.uniform_blocks[0] = {
            .size = 3 * sizeof(mat4),
            .uniforms = {
                [0] = {.name = "transform", .type = SG_UNIFORMTYPE_MAT4},
                [1] = {.name = "perspective", .type = SG_UNIFORMTYPE_MAT4},
                [2] = {.name = "view", .type = SG_UNIFORMTYPE_MAT4},
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
float z = 90;

void mesh_system()
{
    Pool *cameras = get_pool_for_ct(scene, CT_CAMERA);
    Camera *active;
    Transform *c_transform;

    c_foreach(camera, Pool, *cameras)
    {
        Camera *cam = camera.ref->second;
        if (cam->active)
        {
            active = cam;
            c_transform = get_component_for_entity(scene, camera.ref->first, CT_TRANSFORM);
        }
    }

    if (active == NULL)
        return;

    Pool *meshes = get_pool_for_ct(scene, CT_MESH);

    c_forpair(entity, mesh, Pool, *meshes)
    {
        // Deref once because the type of a pool key is *void
        // and we get a reference to that so **void
        Mesh *mesh = *_.mesh;
        Transform *transform = get_component_for_entity(scene, *_.entity, CT_TRANSFORM);

        rotate_euler(transform, (vec3){j / 10.0f, j, 0});
        compute_transform(transform);
        glm_perspective(active->fov, active->aspect_ratio, 0.0f, 1000.0f, active->perspective);
        rotate_euler(c_transform, (vec3){0, 90, 0});

        render_mesh(mesh, transform, active, c_transform, pip);
    }
}

void scene_draw()
{
    j += 0.01f;
    z += 1.0f;

    mesh_system();
}