#ifndef ASCII_MESH_H
#define ASCII_MESH_H
#include <stdlib.h>
#include "triangle.h"

// static mesh: once a vertex is added, it can't be removed!
typedef struct staticMesh_t* StaticMesh;
typedef Position* Vertices;
typedef int* VertexMap;
typedef enum{
	ASCII_MESH_SUCCESS,
	ASCII_MESH_OUT_OF_MEMORY,
	ASCII_MESH_ERROR,
	ASCII_MESH_BAD_ARGUMENT,
	ASCII_MESH_BAD_INDEX,
} MeshStatus;

typedef enum{
	// don't draw diagonals(hides the tringles)
	ASCII_MESH_NO_INNER_SIDES,
	// draw diagonals(shows the tringles)
	ASCII_MESH_INNER_SIDES,
	// draw using triangles
	ASCII_MESH_TRIANGLES,
	// draw using triangle strips
	ASCII_MESH_TRIANGLE_STRIPS
} MeshDrawMode;

// creates a new mesh, sets the initial max size to initial size
StaticMesh staticMeshCreate(size_t initialSize);

// sets the mesh draw mode
void staticMeshSetDrawMode(MeshDrawMode newMode);

// adds a vertex to the mesh
MeshStatus staticMeshAddVertex(StaticMesh mesh, Position point);


// returns the distance between a point and it's closest side to a mesh
double staticmMshGetDistanceToPoint(StaticMesh mesh, Position point);

// returns a positive integer if the point is inside the mesh, 0 if it's not
int staticMeshIsPointInside(StaticMesh mesh, Position point);

// frees a mesh's memory
void staticMeshFree(StaticMesh mesh);

#endif //ASCII_MESH_H
