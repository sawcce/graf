#include "obj_loader.h"
#include "fast_obj.h"

#define i_key Vertex
#define i_type VertexList
#include "stc/cvec.h"

#define i_key int
#define i_type IndexList
#include "stc/cvec.h"

#include "log.h"

Mesh *load_obj(const char *path)
{
    MeshData *mesh_data = malloc(sizeof(MeshData));
    fastObjMesh *obj_data = fast_obj_read(path);

    VertexList vertices = VertexList_init();
    IndexList indices = IndexList_init();

    for (int i = 0; i < obj_data->index_count; i++)
    {
        fastObjIndex index = obj_data->indices[i];
        int positionIndex = index.p;
        int normalIndex = index.n;

        vec3 position, normal;
        glm_vec3_copy(&obj_data->positions[positionIndex * 3], position);
        glm_vec3_copy(&obj_data->normals[normalIndex * 3], normal);

        bool match = false;
        for (int i = 0; i < VertexList_size(&vertices); i++)
        {
            Vertex *v = VertexList_at(&vertices, i);
            if (glm_vec3_eqv(position, v->position) && glm_vec3_eqv(normal, v->normal))
            {
                IndexList_push(&indices, i);
                match = true;
                break;
            }
        }

        if (!match)
        {
            Vertex new_vertex;
            glm_vec3_copy(position, new_vertex.position);
            glm_vec3_copy(normal, new_vertex.normal);
            VertexList_push(&vertices, new_vertex);
            IndexList_push(&indices, VertexList_size(&vertices) - 1);
        }
    }

    mesh_data->elements_amount = obj_data->index_count;
    mesh_data->indices = (sg_range){
        .ptr = indices.data,
        .size = indices._len * sizeof(int),
    };
    mesh_data->vertices = (sg_range){
        .ptr = vertices.data,
        .size = vertices._len * sizeof(Vertex),
    };

    Mesh *new_mesh = make_mesh(*mesh_data);
    free(mesh_data);
    return new_mesh;
}