#include "scene.h"
#include "ecs.h"

#include "cglm/cglm.h"

#include "components/mesh.h"
#include "components/transform.h"
#include "components/camera.h"
#include <omp.h>

#include "log.h"

/* a vertex buffer */
const float vertices[] = {
    // positions            // colors
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f};

const int indices[] = {
    // Front
    0, 1, 2,
    1, 2, 3,
    // Back
    5, 6, 7,
    4, 5, 6,
    // Right
    0, 4, 2,
    4, 2, 6,
    // Left
    1, 5, 3,
    5, 3, 7,
    // Top
    0, 1, 5,
    0, 5, 4,
    // Bottom
    2, 3, 7,
    7, 2, 6};

MeshData triangle_data = {
    .vertices = SG_RANGE(vertices),
    .indices = SG_RANGE(indices),
    .elements_amount = sizeof(indices) / sizeof(int),
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
AppEventQueue event_queue;

void scene_setup()
{
    sapp_lock_mouse(true);
    event_queue = AppEventQueue_init();

    triangle = make_mesh(triangle_data);

    scene = new_scene();

    Transform camera_transform = {
        .position = {0, 0, -4},
    };

    set_rotation_euler(&camera_transform, (vec3){0, GLM_PIf, 0});

    EntityID camera = new_entity(scene);
    assign_to_entity(scene, camera, CT_CAMERA, sizeof(Camera), &(Camera){
                                                                   .active = true,
                                                                   .fov = (70.0f / 360.0f) * 2 * GLM_PI,
                                                                   .aspect_ratio = 4.0f / 3.0f,
                                                               });
    assign_to_entity(scene, camera, CT_TRANSFORM, sizeof(Transform), &camera_transform);

    Transform transform = {
        .position = {0, 0, 0},
        .scale = {0.5f, 0.5f, 0.5f},
    };

    EntityID entity = new_entity(scene);
    assign_to_entity(scene, entity, CT_MESH, sizeof(Mesh), &triangle);
    assign_to_entity(scene, entity, CT_TRANSFORM, sizeof(Transform), &transform);
    assign_to_entity(scene, entity, CT_SPINNING, 0, NULL);

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
        .depth = {
            .write_enabled = true,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
        },
        .shader = shd,
        .layout = {
            .attrs = {[0].format = SG_VERTEXFORMAT_FLOAT3, [1].format = SG_VERTEXFORMAT_FLOAT4},
        },
    });
}

float j = 0;

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

        compute_transform(transform);
        glm_perspective(active->fov, active->aspect_ratio, 0.1f, 1000.0f, active->perspective);

        render_mesh(mesh, transform, active, c_transform, pip);
    }
}

void spinning_system()
{
    Pool *spinning = get_pool_for_ct(scene, CT_SPINNING);
    c_foreach(entity, Pool, *spinning)
    {
        EntityID eID = entity.ref->first;
        Transform *transform = get_component_for_entity(scene, eID, CT_TRANSFORM);
        set_rotation_euler(transform, (vec3){j / 10.0f, j, 0});
    }
}

extern float delta_time_sec;

void camera_move_system(float mouse_x, float mouse_y)
{
    Pool *camera_pool = get_pool_for_ct(scene, CT_CAMERA);
    c_foreach(camera, Pool, *camera_pool)
    {
        Transform *camera_transform = get_component_for_entity(scene, camera.ref->first, CT_TRANSFORM);
        rotate_euler(camera_transform, (vec3){mouse_x * delta_time_sec, mouse_y * delta_time_sec, 0});
    }
}

void input_system()
{
    while (!AppEventQueue_empty(&event_queue))
    {
        sapp_event *event = AppEventQueue_pull(&event_queue);

        switch (event->type)
        {
        case SAPP_EVENTTYPE_MOUSE_MOVE:
            camera_move_system(-event->mouse_dy * 1.0f, -event->mouse_dx * 1.0f);
        }
    }
}

void event(sapp_event *event)
{
    AppEventQueue_push(&event_queue, event);
}

void scene_draw()
{
    j += 0.01f;

#pragma omp parallel
    input_system();
    mesh_system();
    spinning_system();
}