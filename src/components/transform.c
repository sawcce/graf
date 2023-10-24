#include "transform.h"

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