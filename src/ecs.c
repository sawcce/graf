#include "ecs.h"
#include "log.h"

void Component_drop(Pool *self, EntityID key)
{
    void *c = ComponentList_get(&self->list, key)->second;
    if (self->free_method != NULL)
    {
        self->free_method(c);
    }
    free(c);

    ComponentList_erase(&self->list, key);
}

void Entity_drop(Scene *scene, EntityID entity)
{
    TypeSet ts = cmap_entities_get(&scene->entities, entity)->second;
    c_foreach(type, TypeSet, ts)
    {
        Pool *pool = &Pools_get(&scene->pools, *type.ref)->second;
        Component_drop(pool, entity);
    }
}

Scene *new_scene()
{
    Scene *ptr = malloc(sizeof(Scene));
    ptr->pools = Pools_with_capacity(10);

    ptr->entities = cmap_entities_with_capacity(10);
    return ptr;
}
void create_pool_for_ct(Scene *scene, CType type, void (*free_method)(void *))
{
    Pools_insert(&scene->pools, type, (Pool){
                                          .free_method = free_method,
                                          .list = ComponentList_init(),
                                      });
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

    ComponentList_insert(&pool->list, entity, component);
    return component;
}

void *get_component_for_entity(Scene *scene, EntityID entity, CType type)
{
    Pool *pool = &Pools_get_mut(&scene->pools, type)->second;
    return ComponentList_get_mut(&pool->list, entity)->second;
}

Pool *get_pool_for_ct(Scene *scene, CType type)
{
    return &Pools_get(&scene->pools, type)->second;
}
