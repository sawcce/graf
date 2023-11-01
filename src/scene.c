#include "scene.h"
#include "ecs.h"

#include "cglm/cglm.h"

#include "components/mesh.h"
#include "components/transform.h"
#include "components/camera.h"

#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

#include <omp.h>

#include "log.h"

MeshData teapot_data = {
    .id = "teapot",
};

Mesh teapot;
fastObjMesh *mesh;

sg_shader shd;
sg_pipeline pip;

Scene *scene;
AppEventQueue event_queue;

void scene_setup()
{
    mesh = fast_obj_read("res/teapot.obj");
    teapot_data.vertices = (sg_range){
        .ptr = mesh->positions + 3,
        .size = mesh->position_count * 3 * sizeof(int),
    };

    int *indices = malloc(sizeof(int) * mesh->index_count);
    for (int i = 0; i < mesh->index_count; i++)
    {
        fastObjIndex index = mesh->indices[i];
        indices[i] = index.p - 1;
    }

    teapot_data.indices = (sg_range){
        .ptr = indices,
        .size = sizeof(int) * mesh->index_count,
    };

    teapot_data.elements_amount = mesh->index_count;

    teapot = make_mesh(teapot_data);

    sapp_lock_mouse(true);
    event_queue = AppEventQueue_init();

    scene = new_scene();

    create_pool_for_ct(scene, CT_TRANSFORM, NULL);
    create_pool_for_ct(scene, CT_CAMERA, NULL);
    create_pool_for_ct(scene, CT_SPINNING, NULL);
    create_pool_for_ct(scene, CT_MESH, drop_mesh);

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
    assign_to_entity(scene, entity, CT_MESH, sizeof(Mesh), &teapot);
    assign_to_entity(scene, entity, CT_TRANSFORM, sizeof(Transform), &transform);
    assign_to_entity(scene, entity, CT_SPINNING, 0, NULL);

    shd = sg_make_shader(&(sg_shader_desc){
        .vs.source =
            "#version 330\n"
            "layout(location=0) in vec4 position;\n"
            // "layout(location=1) in vec4 color0;\n"
            "uniform mat4 transform;\n"
            "uniform mat4 perspective;\n"
            "uniform mat4 view;\n"
            "out vec4 color;\n"
            "vec4 color0 = vec4(1, 1, 1, 1);\n"
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
            .attrs = {[0].format = SG_VERTEXFORMAT_FLOAT3},
        },
    });
}

void scene_destroy()
{
    c_foreach(entity, cmap_entities, scene->entities)
    {
        Entity_drop(scene, entity.ref->first);
    }

    sg_destroy_shader(shd);
    sg_destroy_pipeline(pip);
}

float j = 0;

void mesh_system()
{
    Pool *cameras = get_pool_for_ct(scene, CT_CAMERA);
    Camera *active;
    Transform *c_transform;

    c_foreach(camera, ComponentList, cameras->list)
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

    c_forpair(entity, mesh, ComponentList, meshes->list)
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
    c_foreach(entity, ComponentList, spinning->list)
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
    c_foreach(camera, ComponentList, camera_pool->list)
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