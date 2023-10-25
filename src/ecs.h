#pragma once

typedef enum
{
    CT_MESH = 1,
    CT_TRANSFORM,
    CT_CAMERA,
} Type;

typedef struct
{
    Type type;
    void *component;
} Component;

typedef struct
{
    Component components[3];
} Entity;

#define i_key int
#define i_val Entity
#define i_tag entity
#include "stc/cmap.h"

typedef struct
{
    cmap_entity entities;
} Scene;
