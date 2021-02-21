#include "staticMesh.h"
#define GROWTH_RATE 2
struct staticMesh_t{
	size_t maxSize;
	size_t vertexCount;
	Vertices vertices;
	size_t mapMaxSize;
	size_t currentMapIndex;
	VertexMap map;
};
static MeshStatus expand(StaticMesh mesh){
	if(mesh->vertexCount < mesh->maxSize)
		return ASCII_MESH_SUCCESS;

	mesh->maxSize *= GROWTH_RATE;	
	Vertices moved = realloc(mesh->vertices, sizeof(*moved) * mesh->maxSize);
	if(moved == NULL){
		free(mesh->vertices);
		return ASCII_MESH_OUT_OF_MEMORY;
	}
    mesh->vertices = moved;
	return ASCII_MESH_SUCCESS;
}
StaticMesh meshCreate(size_t initialSize){
	StaticMesh res = malloc(sizeof(*res));

	if(res == NULL)
		return NULL;

	res->maxSize = initialSize;
	res->vertexCount = 0;
	res->vertices = malloc(sizeof(*res->vertices) * res->maxSize);
	if(res->vertices == NULL){
		free(res);
		return NULL;
	}

	return res;
}
MeshStatus meshAddVertex(StaticMesh mesh, Position point){
	if(mesh == NULL)
		return ASCII_MESH_BAD_ARGUMENT;


	//if we have more than 3 start making triangles
	mesh->vertices[mesh->vertexCount++] = point;	
	
	//will expand if we need more space
	if(expand(mesh) != ASCII_MESH_SUCCESS)
		return ASCII_MESH_OUT_OF_MEMORY;

	return ASCII_MESH_SUCCESS;
}

void meshFree(StaticMesh mesh){
	if(mesh == NULL)
		return;
	free(mesh->vertices);
	free(mesh);
}
