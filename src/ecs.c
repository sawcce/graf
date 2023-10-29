#include "ecs.h"
#include "log.h"

Scene *new_scene()
{
    Scene *ptr = malloc(sizeof(Scene));
    ptr->pools = Pools_with_capacity(10);

    // Preallocates all the pools
    for (int i = 0; i < 5; i++)
    {
        Pools_insert(&ptr->pools, i, Pool_init());
    }

    ptr->entities = cmap_entities_with_capacity(10);
    return ptr;
}

EntityID new_entity(Scene *scene)
{
    EntityID id = rand();

    cmap_entities_insert(&scene->entities, id, TypeSet_init());
    return id;
}

void *assign_to_entity(Scene *scene, EntityID entity, CType type, size_t component_size, void *init)
{
    cmap_entities_value *components = cmap_entities_get_mut(&scene->entities, entity);
    TypeSet_push(&components->second, type);

    Pool *pool = &Pools_get_mut(&scene->pools, type)->second;
    void *component = malloc(component_size);
    memcpy(component, init, component_size);

    Pool_insert(pool, entity, component);
    return component;
}

void *get_component_for_entity(Scene *scene, EntityID entity, CType type)
{
    Pool *pool = &Pools_get_mut(&scene->pools, type)->second;
    return Pool_get_mut(pool, entity)->second;
}

Pool *get_pool_for_ct(Scene *scene, CType type)
{
    return &Pools_get(&scene->pools, type)->second;
}
