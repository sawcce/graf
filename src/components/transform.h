#pragma once

#include "cglm/cglm.h"

typedef struct
{
    vec3 position;
    vec3 scale;
    versor rotation;
    mat4 computed;
} Transform;

void rotate_euler(Transform *transform, vec3 euler);
void compute_transform(Transform *transform);