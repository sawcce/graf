#include "transform.h"

// WARNING: Operations on transforms do not update
// the computed matrix. It is up to the developer
// to manually call `compute_transform` in order
// to update the `computed` field.
// This is done so that multiple operations
// can be done in a row without costing too much
// operations.

// Rotates a transform component using euler angles
void set_rotation_euler(Transform *transform, vec3 euler)
{
    mat4 rotation_matrix;
    glm_euler_xyz(euler, rotation_matrix);

    versor new_rotation;
    glm_mat4_quat(rotation_matrix, new_rotation);
    glm_vec4_copy(new_rotation, transform->rotation);
}

void rotate_euler(Transform *transform, vec3 euler)
{
    mat4 rotation_matrix;
    glm_euler_xyz(euler, rotation_matrix);

    versor new_rotation;
    glm_mat4_quat(rotation_matrix, new_rotation);
    glm_quat_mul(transform->rotation, new_rotation, transform->rotation);
}

// Computes the transform matrix of a `Transform` component
// and puts the result into the `computed` field
// of said component
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