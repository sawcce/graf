#pragma once
#include "transform.h"

typedef struct
{
    bool active;
    float_t fov;
    float_t aspect_ratio;
    mat4 perspective;
} Camera;