#pragma once

#define EntityID int64_t

typedef enum
{
    CT_MESH = 1,
    CT_TRANSFORM,
    CT_CAMERA,
} CType;

// cset_ctype: A hashset of component types
#define i_key CType
#define i_type TypeSet
#include "stc/cset.h"

// cmap_entity: A map of EntityIDs and the set of the Component Types
// assigned to them
#define i_key EntityID
#define i_val TypeSet
#define i_tag entities
#include "stc/cmap.h"

#define i_key EntityID
#define i_val void *
#define i_type Pool
#include "stc/cmap.h"

#define i_key CType
#define i_val Pool
#define i_type Pools
#include "stc/cmap.h"

typedef struct
{
    Pools pools;
    cmap_entities entities;
} Scene;

Scene *new_scene();

EntityID new_entity(Scene *scene);
void assign_to_entity(Scene *scene, EntityID entity, CType type, size_t component_size, void *init);
void *get_component_for_entity(Scene *scene, EntityID entity, CType type);

// Gets the pool for a component type using the
// Component Type ID. Returns `null` if the
// pool does not exist.
Pool *get_pool_for_ct(Scene *scene, CType type);
